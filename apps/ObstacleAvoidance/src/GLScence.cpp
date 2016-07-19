#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <qfile.h>
#include <qimage.h>
#include <QGLViewer/qglviewer.h>	//pour qglwidget
#include <QGLViewer/vec.h>
#include <QMessageBox>

#include <base/Svar/Svar.h>
#include <base/Path/Path.h>

#include "GLScence.h"

#define TRN_NUM_TILES 5

#define SKY_SIDES 6

#define SKY_FRONT  0
#define SKY_BACK   1
#define SKY_RIGHT  2
#define SKY_LEFT   3
#define SKY_TOP    4
#define SKY_BOTTOM 5

using namespace std;

// structure contenant le hauteur du terrain
struct HEIGHTMAP
{
  unsigned char* arrayHeightMap;	// la liste
  int sizeHeightMap;		// sa taille (puissance 2)
};

//structure contenant les positions d'ombres
struct LIGHTMAP
{
  unsigned char* arrayLightMap;
  int sizeLightMap;
};


// 4 types de textures bases selon les hauteurs du terrain
enum TEXTURETYPE
  {
    LOWEST= 0,		//sable
    MEDIUM,			//herbe
    HIGH,			//pierres
    HIGHEST		 	//sommet de montagne
  };

// pour melanger les textures de bases, on definit des intervalles d'hauteurs acceptable pour le placement
struct TEXTUREREGION
{
  int lowHeight;			// acceptance 0
  int optimalHeight;		// acceptance 100
  int highHeight;			// acceptance 0
};

// structure contenant les textures de base et les infos correspondantes
struct TEXTURE
{
  TEXTUREREGION region[TRN_NUM_TILES];
  QImage data[TRN_NUM_TILES];
  int numTextures;
};


// classe abstraite
class TERRAIN
{
protected:
  HEIGHTMAP heightMap;
  float scaleHeightMap;		// facteur d'echelle pour surelever le terrain
  float scaleSize;		//facteur d'echelle pour la taille du terrain

  TEXTURE textures;	//gestion de textures
  QImage myTexture;		//texture de couleur
  QImage myDetailMap;		//texture de detail
  int repeatDetailMap;
  bool haveMultitexture;
  bool paintTextures;

  //gestion de lumiere / ombres
  LIGHTMAP lightMap;
  float rLight,gLight,bLight;
  float minBrightness, maxBrightness;
  float lightSoftness;
  int directionX, directionZ;
  bool paintLighting;

  //fcts. d'aide pour la generation de terrain fractale (filtrage)
  void NormalizeTerrain( float* heightData );
  void SmoothTerrain(float* heightData, int kernelSize);
  void Smooth1DTerrain(float* heightData, int kernelSize);

  //fcts. d'aide de generation de textures
  float RegionPercent( int type, unsigned char height );
  void GetTexCoords( QImage texture, unsigned int* x, unsigned int* y );
  unsigned char InterpolateHeight( int x, int z, float heightToTexRatio );

public:
  int sizeHeightMap;

  virtual void Render( void )= 0;			//fct. abstraite, a remplir par implementations

  bool LoadHeightMap( const QString& szFilename, int iSize );	//gestion des cartes d'hauteurs de terrain
  bool SaveHeightMap( const QString& szFilename );
  bool UnloadHeightMap( void );

  //generation de terrain fractale
  bool MakeTerrainFault( int size, int iterations, int min, int max, int smooth );  //smooth=1,3,5,7

  //generation de textures
  unsigned int textureColorID;		//pour identifier les textures aupres de opengl
  unsigned int textureDetailID;
  void GenerateTextureMap( unsigned int size );
  bool LoadTexture( const QString& filename );
  bool LoadDetailMap( const QString& filename );

  //fonctions de lumiere
  void CalculateLighting( void );
  void StepLightingDirection(void);


  //determiner l'echelle d'hauteur
  inline void SetHeightScale( float scale )
  {	scaleHeightMap = scale;	}

  inline void SetSizeScale( float scale )
  {	scaleSize = scale;	}

  inline void SetHeightAtPoint( unsigned char height, int X, int Z)
  {	heightMap.arrayHeightMap[( Z*sizeHeightMap )+X]= height;	}

  //renvoyer hauteur sans echelle
  inline unsigned char GetTrueHeightAtPoint( int X, int Z )
  {	return ( heightMap.arrayHeightMap[( Z*sizeHeightMap )+X] );	}

  inline float GetScaledHeightAtPoint( int X, int Z )
  {
    if (X>=sizeHeightMap) X=sizeHeightMap-1;		//eviter les effets de bord
    if (Z>=sizeHeightMap) Z=sizeHeightMap-1;
    return ( ( float )( heightMap.arrayHeightMap[( Z*sizeHeightMap )+X] )*scaleHeightMap );
  }

  inline bool SaveTextureMap( const QString& filename )
  {
    if( !myTexture.isNull( ) )
      return ( myTexture.save( filename , "BMP"));

    return false;
  }

  inline void UnloadTexture( void )
  {
#if QT_VERSION < 0x040000
    myTexture.reset( );
#endif
  }

  inline void UnloadDetailMap( void )
  {
#if QT_VERSION < 0x040000
    myDetailMap.reset( );
#endif
  }

  inline void DoMultitexturing( bool have )
  {	haveMultitexture= have;	}

  inline void DoTexturing( bool doIt )
  {	paintTextures= doIt;	}

  inline void DoLighting( bool doIt )
  {	paintLighting= doIt;	}

  inline bool isTexture()
  {	return paintTextures;}

  inline bool isLighted()
  {	return paintLighting;}

  inline bool LoadTile( TEXTURETYPE type, const QString& filename )
  {	return textures.data[type].load( filename ); }

  inline void UnloadTile(TEXTURETYPE type)
  {
#if QT_VERSION < 0x040000
    textures.data[type].reset();
#else
    Q_UNUSED(type)
#endif
  }

  inline void UnloadAllTextures( void )
  {
    UnloadTile( LOWEST );
    UnloadTile( MEDIUM );
    UnloadTile( HIGH );
    UnloadTile( HIGHEST );
  }

  //gestion des fct. pourries de QIMAGE
  inline void GetColor( const QImage& imageData, unsigned int x, unsigned  int y, unsigned char* Red, unsigned char* Green, unsigned char* Blue )
  {
    QRgb colors = imageData.pixel((int)x,(int)y);
    *Red = (unsigned char)qRed(colors);
    *Green = (unsigned char)qGreen(colors);
    *Blue = (unsigned char)qBlue(colors);
  }


  inline void SetColor( QImage& imageData, unsigned int x, unsigned  int y, unsigned char Red, unsigned char Green, unsigned char Blue )
  {
    imageData.setPixel( (int)x, (int)y, qRgb((int)Red ,(int)Green , (int)Blue ) );
  }


  unsigned char Limit( float value )
  {
    if( value>255 )
      return 255;
    else if( value<0 )
      return 0;
    return (unsigned char)value;
  }

  inline void SetBrightnessAtPoint( int x, int z, unsigned char brightness )
  {	lightMap.arrayLightMap[( z*lightMap.sizeLightMap )+x]= brightness;	}

  inline unsigned char GetBrightnessAtPoint( int x, int z )
  {	return lightMap.arrayLightMap[( z*lightMap.sizeLightMap )+x];	}

  inline void SetLightColor( float r, float g, float b )
  {	rLight=r; gLight=g; bLight=b;	}

  //avec ce modele d'ombrage simpliste, on a meme pas besoin de l'hauteur de la source de lumiere...
  inline void SetLightDirection( int dirX, int dirZ)
  {
    directionX= dirX;
    directionZ= dirZ;
  }

  inline void GetLightDirection( int* dirX, int* dirZ)
  {
    *dirX=directionX;
    *dirZ=directionZ;
  }

  inline bool UnloadLightMap(void)
  {
    if( lightMap.arrayLightMap )
      {
    delete[] lightMap.arrayLightMap;
    lightMap.sizeLightMap = 0;
    return true;
      }
    return false;
  }

  TERRAIN( void )
  {
    repeatDetailMap = 8;			//A REVISER
    SetLightColor(1.0f,1.0f,1.0f);
    minBrightness = 0.2f;		//valeurs qui marchent bien
    maxBrightness = 0.9f;
    lightSoftness = 10;
    paintLighting = true;
    directionX = -1;
    directionZ = 0;
    scaleHeightMap = 0.25f;
    scaleSize = 1.0f;		//8.0f
    heightMap.arrayHeightMap=NULL;
    lightMap.arrayLightMap=NULL;
  }
  virtual  ~TERRAIN( void )
  {	}
};


bool TERRAIN::LoadHeightMap(const QString& filename, int size)
{
  if( heightMap.arrayHeightMap )
    UnloadHeightMap( );

  QFile pFile(filename);
#if QT_VERSION >= 0x040000
    if (!pFile.open(QIODevice::ReadOnly))
#else
    if (!pFile.open(IO_ReadOnly))
#endif
    return false;

  heightMap.arrayHeightMap = new unsigned char [size*size];

  if( heightMap.arrayHeightMap==NULL )
    return false;

  //lire la carte d'hauteurs, format RAW
  QDataStream in(&pFile);
  for (int i=0; i<sizeHeightMap*sizeHeightMap; i++)
    in >> heightMap.arrayHeightMap[i];

  pFile.close();

  sizeHeightMap = size;

  return true;
}


bool TERRAIN::SaveHeightMap(const QString& filename )
{
  QFile pFile(filename);
#if QT_VERSION >= 0x040000
    if (!pFile.open(QIODevice::WriteOnly))
#else
    if (!pFile.open(IO_WriteOnly))
#endif
    return false;

  if( heightMap.arrayHeightMap==NULL )
    return false;

  QDataStream out(&pFile);
  for (int i=0; i<sizeHeightMap*sizeHeightMap; i++)
    out << heightMap.arrayHeightMap[i];

  pFile.close();

  return true;
}

bool TERRAIN::UnloadHeightMap( void )
{
  if( heightMap.arrayHeightMap )
    {
      delete[] heightMap.arrayHeightMap;

      sizeHeightMap= 0;
    }

  return true;
}

void TERRAIN::Smooth1DTerrain(float* heightData, int kernelSize)  //filtrage unidirectionel (moins lisse)
{
  int i,j,kernelPos,effectSize,base;
  float sum;
  float* temp = new float [sizeHeightMap*sizeHeightMap];
  for (i=0;i<sizeHeightMap*sizeHeightMap; i++ )
    {
      j = 0;
      sum = 0;
      effectSize=0;
      base = i-(kernelSize/2*sizeHeightMap);
      do
    {
      kernelPos = base + j*sizeHeightMap;
      if (kernelPos>=0 && kernelPos < sizeHeightMap*sizeHeightMap)
        {
          sum += heightData[kernelPos];
          effectSize++;
        }
      j++;
    } while (j<kernelSize);
      temp[i]=(float)sum/effectSize;
    }
  for (i=0;i<sizeHeightMap*sizeHeightMap; i++ )
    heightData[i]=temp[i];

  delete temp;
}

void TERRAIN::SmoothTerrain(float* heightData, int kernelSize)	//filtrage 2D moyenne kernelSize^2, bords inclus
{
  int i,j,kernelPos,effectSize,base;
  float sum;
  float* temp = new float [sizeHeightMap*sizeHeightMap];
  for (i=0;i<sizeHeightMap*sizeHeightMap; i++ )
    {
      j = 0;
      sum = 0;
      effectSize=0;
      base = i-(kernelSize/2*sizeHeightMap)-(kernelSize/2);
      do
    {
      kernelPos = base + (j%kernelSize) + (j/kernelSize)*sizeHeightMap;
      if (kernelPos>=0 && kernelPos < sizeHeightMap*sizeHeightMap)
        {
          sum += heightData[kernelPos];
          effectSize++;
        }
      j++;
    } while (j<kernelSize*kernelSize);
      temp[i]=(float)sum/effectSize;
    }
  for (i=0;i<sizeHeightMap*sizeHeightMap; i++ )
    heightData[i]=temp[i];
  delete[] temp;
}


void TERRAIN::NormalizeTerrain( float* heightData )
{
  float min, max;
  float height;
  int i;

  min= heightData[0];
  max= heightData[0];

  //chercher minimum et maximum des donnees
  for( i=1; i<sizeHeightMap*sizeHeightMap; i++ )
    {
      if( heightData[i]>max )
    max= heightData[i];

      else if( heightData[i]<min )
    min= heightData[i];
    }

  //si terrain est plat: rien a faire
  if( max<=min )
    return;

  height= max-min;

  //scaler les valeurs a une echelle de 0..255
  for( i=0; i<sizeHeightMap*sizeHeightMap; i++ )
    heightData[i]= ( ( heightData[i]-min )/height )*255.0f;
}


bool TERRAIN::MakeTerrainFault( int size, int iterations, int min, int max, int smooth )
{
  float* tempBuffer;
  int currentIteration;
  int height;
  int randX1, randZ1;
  int randX2, randZ2;
  int dirX1, dirZ1;
  int dirX2, dirZ2;
  int x, z;
  int i;
  srand( time(NULL) );

  if( heightMap.arrayHeightMap )
    UnloadHeightMap( );

  sizeHeightMap= size;

  //reserver de la memoire
  heightMap.arrayHeightMap = new unsigned char [sizeHeightMap*sizeHeightMap];
  tempBuffer= new float [sizeHeightMap*sizeHeightMap];

  if( heightMap.arrayHeightMap==NULL )
    {
      return false;
    }

  if( tempBuffer==NULL )
    {
      return false;
    }

  for( i=0; i<sizeHeightMap*sizeHeightMap; i++ )
      tempBuffer[i]= 0;

  for( currentIteration=0; currentIteration<iterations; currentIteration++ )
  {
      //choisir la hauteur pour cette traversee avec interp. lineaire,
      //..on reduit la taille des changements effectues avec chaque iteration (grossier->detaille)
      //.. meme si toujours le meme point est choisi, on ne depasse pas la hauteur max.
      height= max - ( ( max-min )*currentIteration )/iterations;

      //choisir deux points de la carte aleatoirement
      randX1= rand( )%sizeHeightMap;
      randZ1= rand( )%sizeHeightMap;

      //...tant qu'ils sont differents
      do
      {
          randX2= rand( )%sizeHeightMap;
          randZ2= rand( )%sizeHeightMap;
      } while ( randX2==randX1 && randZ2==randZ1 );


      //le vecteur de la ligne aleatoire qui divise la carte en deux
      dirX1= randX2-randX1;
      dirZ1= randZ2-randZ1;

      for( z=0; z<sizeHeightMap; z++ )
      {
          for( x=0; x<sizeHeightMap; x++ )
          {
              //vecteur de position du point traite actuellement
              dirX2= x-randX1;
              dirZ2= z-randZ1;
              //utiliser le signe du produit croise pour decide si on exhausse la hauteur de ce point
              if( ( dirX2*dirZ1 - dirX1*dirZ2 )>0 )
                  tempBuffer[( z*sizeHeightMap )+x]+= ( float )height;
          }
      }

      //effectuer erosion
      SmoothTerrain(tempBuffer,smooth);	//MOYENNE
      //Smooth1DTerrain(tempBuffer,smooth);
  }

  //adapter l'echelle de valeurs
  NormalizeTerrain( tempBuffer );

  //sauvegarder le terrain cree dans la variable de la classe TERRAIN,
  //.. on n'a plus besoin de la precision float
  for( z=0; z<sizeHeightMap; z++ )
  {
      for( x=0; x<sizeHeightMap; x++ )
          SetHeightAtPoint( ( unsigned char )tempBuffer[( z*sizeHeightMap )+x], x, z );
  }

  if( tempBuffer )
  {
      delete[] tempBuffer;
  }
  return true;
}

//calculer combien de pourcent de visibilite un type de texture devrait occuper a une certaine hauteur
float TERRAIN::RegionPercent( int type, unsigned char height )
{
  float temp1, temp2;

  //afficher la texture la plus basse avec luminosite maximale au-dessous de la hauteur optimale
  //.. cad. on a pas de texture correspondant a cette hauteur
  // .. et il se peut qu'on a pas charge toutes les textures...
  if( !textures.data[LOWEST].isNull( ) )
    {
      if( type==LOWEST && height<textures.region[LOWEST].optimalHeight )
    return 1.0f;
    }
  else if( !textures.data[MEDIUM].isNull( ) )
    {
      if( type==MEDIUM && height<textures.region[MEDIUM].optimalHeight )
    return 1.0f;
    }
  else if( !textures.data[HIGH].isNull( ) )
    {
      if( type==HIGH && height<textures.region[HIGH].optimalHeight )
    return 1.0f;
    }
  else if( !textures.data[HIGHEST].isNull( ) )
    {
      if( type==HIGHEST && height<textures.region[HIGHEST].optimalHeight )
    return 1.0f;
    }

  //hauteur est au-dessous de l'intervalle d'affichage de la texture souhaite: on n'en utilise rien
  if( height<textures.region[type].lowHeight )
    return 0.0f;

  //hauteur est au-dessus de l'intervalle d'affichage de la texture souhaite: on n'en utilise rien
  else if( height>textures.region[type].highHeight )
    return 0.0f;

  //hauteur est au-dessous de la hauteur optimale: on calcule le pourcentage d'utilisation
  if( height<textures.region[type].optimalHeight )
    {
      temp1= ( float )height-textures.region[type].lowHeight;
      temp2= ( float )textures.region[type].optimalHeight-textures.region[type].lowHeight;
      //repartition: hauteur par rapport a la taille de l'intervalle
      return ( temp1/temp2 );
    }

  //hauteur est egale a l'hauteur ideale: on l'affiche a luminosite maximale
  else if( height==textures.region[type].optimalHeight )
    return 1.0f;

  //hauteur est au-dessus de la hauteur optimale: on calcule le pourcentage d'utilisation
  else if( height>textures.region[type].optimalHeight )
    {
      temp1= ( float )textures.region[type].highHeight-textures.region[type].optimalHeight;
      return ( ( temp1-( height-textures.region[type].optimalHeight ) )/temp1 );
    }

  //default
  return 0.0f;
}

// on repete la texture plusieurs fois si on veut remplir un terrain plus grand que la texture elle-meme;
// .. afin de savoir quelle pixel de la texture a afficher a une position sur dans le terrain
// .. (en tenant compte des repetitions et du fait qu'un pixel de la texture doit harmoniser avec ses voisins),..
// .. on teste combien de repetitions ont deja ete faites et on renvoie la position locale a utiliser sur la texture

void TERRAIN::GetTexCoords( QImage texture, unsigned int* x, unsigned int* y )
{
  unsigned int width = texture.width( );
  unsigned int height= texture.height( );
  int repeatX= -1;
  int repeatY= -1;
  int i= 0;

  //determiner nombre de repetitions de la texture en x
  while( repeatX==-1 )
    {
      i++;
      if( *x<( width*i ) )
    repeatX= i-1;
    }

  i= 0;

  //determiner nombre de repetitions de la texture en y
  while( repeatY==-1 )
    {
      i++;
      if( *y<( height*i ) )
    repeatY= i-1;
    }

  //on renvoie la position locale contenant le pixel a utiliser sur la texture
  *x= *x-( width*repeatX );
  *y= *y-( height*repeatY );
}

//a l'interieur de chaque texture, on tient compte des variations d'hauteur
//.. pour ne pas avoir des regions de couleur identique bien que la hauteur varie
//.. pour cela, on doit savoir la hauteur interpole a chaque position
// heightToTexRatio: relation taille de map d'hauteur / taille de texture
unsigned char TERRAIN::InterpolateHeight( int x, int z, float heightToTexRatio )
{
  unsigned char  Low,  HighX,  HighZ;
  float  X,  Z;
  float scaledX= x*heightToTexRatio;		//echelle d'interpolation necessaire
  float scaledZ= z*heightToTexRatio;
  float interpolation;

  //borne inf
  Low= GetTrueHeightAtPoint( ( int )scaledX, ( int )scaledZ );

  //borne sup x
  if( ( scaledX+1 )>sizeHeightMap )
    return  Low;
  else
    HighX= GetTrueHeightAtPoint( ( int )scaledX+1, ( int )scaledZ );

  //valeur interpolee x
  interpolation= ( scaledX-( int )scaledX );
  X= ( (  HighX- Low )*interpolation )+ Low;

  //borne sup z
  if( ( scaledZ+1 )>sizeHeightMap )
    return  Low;
  else
    HighZ= GetTrueHeightAtPoint( ( int )scaledX, ( int )scaledZ+1 );

  //valeur interpolee z
  interpolation= ( scaledZ-( int )scaledZ );
  Z= ( (  HighZ- Low )*interpolation )+ Low;

  //moyenne des deux ~= approx. de la vraie hauteur
  return ( ( unsigned char )( (  X+ Z )/2 ) );
}

//creer une carte de texture en melangeant les quatres types de textures de base
void TERRAIN::GenerateTextureMap( unsigned int size )
{
    unsigned char Red, Green, Blue;
    unsigned int tempID;
    unsigned int x, z;
    unsigned int  TexX,  TexZ;
    float totalRed, totalGreen, totalBlue;
    float blend[4];
    float mapRatio;
    int lastHeight;
    int i;

    //determiner le nombre de textures de bases presentes
    textures.numTextures= 0;
    for( i=0; i<TRN_NUM_TILES; i++ )
    {
        if( !textures.data[i].isNull( ) )
            textures.numTextures++;
    }

    //determiner les intervalles d'affichage pour chaque type de texture de base
    lastHeight= -1;
    for( i=0; i<TRN_NUM_TILES; i++ )
    {
        if( !textures.data[i].isNull( ) )
        {
            //on a trois valeurs a determiner: inf, opt,sup
            textures.region[i].lowHeight=lastHeight+1;
            lastHeight+= 255/textures.numTextures;
            textures.region[i].optimalHeight=lastHeight;
            textures.region[i].highHeight= ( lastHeight-textures.region[i].lowHeight )+lastHeight;
        }
    }

#if QT_VERSION < 0x040000
    myTexture.create( size, size, 32 );
#else
    myTexture = QImage(size, size, QImage::Format_ARGB32);
#endif

    //determiner relation entre resolution de la carte d'hauteur et la res. de la texture
    //.. en general, la texture aura une res. plus elevee
    mapRatio= ( float )sizeHeightMap/ size;

    //creation de texture
    for( z=0; z< size; z++ )
    {
        for( x=0; x< size; x++ )
        {
            totalRed  = 0.0f;
            totalGreen= 0.0f;
            totalBlue = 0.0f;

            //pour chaque texture de base
            for( i=0; i<TRN_NUM_TILES; i++ )
            {
                if( !textures.data[i].isNull( ) )
                {
                    TexX= x;
                    TexZ= z;

                    //quel pixel de texture a choisir pour cette position sur la carte?
                    GetTexCoords( textures.data[i], &TexX, &TexZ );

                    //quelle est la couleur actuelle a cet endroit?
                    GetColor( textures.data[i], TexX,  TexZ, &Red, &Green, &Blue );

                    //combien de pourcent de cette texture de base faut-il ajouter a cet endroit?
                    //.. on interpole la vraie hauteur pour avoir des textures plus realistes
                    blend[i]= RegionPercent( i, Limit( InterpolateHeight( x, z, mapRatio ) ) );

                    //ajouter ce pourcentage a la couleur
                    totalRed  += Red*blend[i];
                    totalGreen+= Green*blend[i];
                    totalBlue += Blue*blend[i];
                }
            }

            //modifier la couleur de la texture a cet endroit, limiter les valeurs a 0..255
            SetColor( myTexture, x, z, Limit( totalRed ),Limit( totalGreen ),Limit( totalBlue ) );
        }
    }

    //construire la texture
    glGenTextures( 1, &tempID );
    glBindTexture( GL_TEXTURE_2D, tempID );
    //utiliser la moyenne ponderee lineairement pour elargir ou reduire les textures
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );


    //le plus grand mystere de tout le code: pour une raison completement pourrie, QImage inverse
    //.. les canaux rouge et bleu lors de l'ecriture dans l'image; mais si j'utilise QGLWidget::convertToGLFormat,
    //.. l'image est inversee; la solution swapRGB marche (apres 3h de recherche desesperee) pour une raison mysterieuse
#if QT_VERSION < 0x040000
    myTexture = myTexture.swapRGB();
#else
    myTexture = myTexture.rgbSwapped();
#endif

    //definir la source de donnees pour la texture
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size,  size, 0, GL_RGBA, GL_UNSIGNED_BYTE, myTexture.bits() );

    //associer un no. ID a la texture
    textureColorID =  tempID;

    myTexture.save("texture.bmp","BMP");
}

//on calcule les positions d'ombrages
//..idee de la methode: si entre un point et la source de lumiere, dans la direction
//..de la lumiere, il y a un autre element du terrain, ce point se trouve dans l'ombre
//.. pour cela, on soustrait l'hauteur du vertex voisin de l'hauteur du vertex actuel
//.. CEPENDANT, on est limite a des changements de position de lumiere par pas de 45°
//.. et on ne tient pas compte de l'hauteur de la source de lumiere! (seulement le vertex directement a cote compte)
void TERRAIN::CalculateLighting( void )
{
  float shade;
  int x, z;

  if( lightMap.sizeLightMap!=sizeHeightMap || lightMap.arrayLightMap==NULL )
  {
      delete[] lightMap.arrayLightMap;
      lightMap.arrayLightMap= new unsigned char [sizeHeightMap*sizeHeightMap];
      lightMap.sizeLightMap= sizeHeightMap;
  }

  //pour chaque vertex
  for( z=0; z<sizeHeightMap; z++ )
  {
      for( x=0; x<sizeHeightMap; x++ )
      {
          //pour ne pas depasser des bornes
          if( z>=directionZ && x>=directionX )
          {
              //comparer les hauteurs, et on rend plus doux les frontieres
              //ici, on ne fait PAS de calcul genre "tracer les rayons"...
              shade= 1.0f-( GetTrueHeightAtPoint( x-directionX, z-directionZ ) -
                            GetTrueHeightAtPoint( x, z ) )/lightSoftness;
          }
          else
              shade= 1.0f;

          if( shade<minBrightness )
              shade= minBrightness;
          if( shade>maxBrightness )
              shade= maxBrightness;

          SetBrightnessAtPoint( x, z, ( unsigned char )( shade*255 ) );
      }
  }
}

//tourner la lumiere par un pas de 45°
void TERRAIN::StepLightingDirection(void)
{
  if ((directionX==-1)&&(directionZ==-1)) {directionX++;}
  else if ((directionX==0)&&(directionZ==-1)) {directionX++;}
  else if ((directionX==1)&&(directionZ==-1)) {directionZ++;}
  else if ((directionX==1)&&(directionZ==0)) {directionZ++;}
  else if ((directionX==1)&&(directionZ==1)) {directionX--;}
  else if ((directionX==0)&&(directionZ==1)) {directionX--;}
  else if ((directionX==-1)&&(directionZ==1)) {directionZ--;}
  else if ((directionX==-1)&&(directionZ==0)) {directionZ--;}
}

bool TERRAIN::LoadTexture( const QString& filename )
{
  int type;
  if (!myTexture.load(filename))
    return false;

  QImage temp = QGLWidget::convertToGLFormat(myTexture);

  if( temp.depth()==24 )	//devrait etre toujours 32
    type= GL_RGB;
  else
    type= GL_RGBA;

  //construire les textures openGL
  glGenTextures( 1, &textureColorID );
  glBindTexture( GL_TEXTURE_2D, textureColorID );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glTexImage2D( GL_TEXTURE_2D, 0, type, temp.width(), temp.height(), 0, type, GL_UNSIGNED_BYTE, temp.bits() );
  return true;
}

//besoin d'un fonctionnement different car mipmap utilise
bool TERRAIN::LoadDetailMap( const QString& filename )
{
  int type;
  if (!myDetailMap.load(filename))
    return false;

  QImage temp = QGLWidget::convertToGLFormat(myDetailMap);

  if( temp.depth()==24 )	//devrait etre toujours 32
    type= GL_RGB;
  else
    type= GL_RGBA;

  //construire les textures openGL
  glGenTextures( 1, &textureDetailID );
  glBindTexture( GL_TEXTURE_2D, textureDetailID );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  gluBuild2DMipmaps( GL_TEXTURE_2D, type, temp.width(), temp.height(), type, GL_UNSIGNED_BYTE, temp.bits() );
  return true;
}


class SKY
{
private:
  QImage textures[SKY_SIDES];	//les cotes du ciel
  GLuint texIDs[SKY_SIDES];		//les IDs opengl pour chaque texture
  qglviewer::Vec vecMin, vecMax, vecCenter;
  bool iwantsky;

public:
  SKY() { iwantsky = false; }

  bool LoadTexture( int side, const QString& filename );

  void Render();

  //parametres du ciel
  void Set( float X, float Y, float Z, float size )
  {
    vecCenter.setValue( X, Y, Z );

    vecMin.setValue( 0, 0, 0 );
    vecMax.setValue( size, size, size );
  }

  void switchSky() { iwantsky = !iwantsky; }
  bool wantSky() { return iwantsky; }
};

bool SKY::LoadTexture( int side, const QString& filename )
{
  QImage image;
  if (image.load(filename))
    {
      textures[side] = QGLWidget::convertToGLFormat(image);

      //construire les textures openGL
      glGenTextures( 1, &(texIDs[side]) );
      glBindTexture( GL_TEXTURE_2D, (texIDs[side]) );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

      //les deux commandes suivantes servent a elargir la texture pq. la surface soit remplie;
      //.. clamp_to_edge = repeter la derniere ligne de la texture (marche bien ici)
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textures[side].width(), textures[side].height(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, textures[side].bits() );
    }
  else
    {
      qWarning("Sky texture loading failed for side %d from %s",side,filename.toStdString().c_str());
      return false;
    }
  return true;
}

void SKY::Render()
{
  //on ne veut pas que le background disparaisse (POURTANT, IL DISPARAIT!! J'y arrive pas...)
  //glDisable( GL_CULL_FACE );
  //glDisable( GL_DEPTH_TEST );
  //glDepthMask( GL_FALSE );
  glDisable(GL_LIGHTING);

  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  glEnable( GL_TEXTURE_2D );
  glPushMatrix( );
  glRotatef(90,1,0,0);
  //on construit a partir de la position de la camera (ou du milieu...)
  glTranslatef( vecCenter[0], vecCenter[1], vecCenter[2] );

  //face
  glBindTexture( GL_TEXTURE_2D, texIDs[SKY_FRONT] );
  glBegin( GL_TRIANGLE_FAN );
  glTexCoord2f( 1.0f, 1.0f ); glVertex3f( vecMax[0], vecMax[1], vecMax[2] );
  glTexCoord2f( 1.0f, 0.0f ); glVertex3f( vecMax[0], vecMin[1], vecMax[2] );
  glTexCoord2f( 0.0f, 0.0f ); glVertex3f( vecMin[0], vecMin[1], vecMax[2] );
  glTexCoord2f( 0.0f, 1.0f ); glVertex3f( vecMin[0], vecMax[1], vecMax[2] );
  glEnd( );

  //derriere
  glBindTexture( GL_TEXTURE_2D, texIDs[SKY_BACK]);
  glBegin( GL_TRIANGLE_FAN );
  glTexCoord2f( 1.0f, 1.0f ); glVertex3f( vecMin[0], vecMax[1], vecMin[2] );
  glTexCoord2f( 1.0f, 0.0f ); glVertex3f( vecMin[0], vecMin[1], vecMin[2] );
  glTexCoord2f( 0.0f, 0.0f ); glVertex3f( vecMax[0], vecMin[1], vecMin[2] );
  glTexCoord2f( 0.0f, 1.0f ); glVertex3f( vecMax[0], vecMax[1], vecMin[2] );
  glEnd( );

  //droite
  glBindTexture( GL_TEXTURE_2D, texIDs[SKY_RIGHT] );
  glBegin( GL_TRIANGLE_FAN );
  glTexCoord2f( 1.0f, 1.0f ); glVertex3f( vecMax[0], vecMax[1], vecMin[2] );
  glTexCoord2f( 1.0f, 0.0f ); glVertex3f( vecMax[0], vecMin[1], vecMin[2] );
  glTexCoord2f( 0.0f, 0.0f ); glVertex3f( vecMax[0], vecMin[1], vecMax[2] );
  glTexCoord2f( 0.0f, 1.0f ); glVertex3f( vecMax[0], vecMax[1], vecMax[2] );
  glEnd( );

  //gauche
  glBindTexture( GL_TEXTURE_2D, texIDs[SKY_LEFT] );
  glBegin( GL_TRIANGLE_FAN );
  glTexCoord2f( 1.0f, 1.0f ); glVertex3f( vecMin[0], vecMax[1], vecMax[2] );
  glTexCoord2f( 1.0f, 0.0f ); glVertex3f( vecMin[0], vecMin[1], vecMax[2] );
  glTexCoord2f( 0.0f, 0.0f ); glVertex3f( vecMin[0], vecMin[1], vecMin[2] );
  glTexCoord2f( 0.0f, 1.0f ); glVertex3f( vecMin[0], vecMax[1], vecMin[2] );
  glEnd( );

  //au-dessus
  glBindTexture( GL_TEXTURE_2D, texIDs[SKY_TOP] );
  glBegin( GL_TRIANGLE_FAN );
  glTexCoord2f( 0.0f, 0.0f ); glVertex3f( vecMin[0], vecMax[1], vecMax[2] );
  glTexCoord2f( 0.0f, 1.0f ); glVertex3f( vecMin[0], vecMax[1], vecMin[2] );
  glTexCoord2f( 1.0f, 1.0f ); glVertex3f( vecMax[0], vecMax[1], vecMin[2] );
  glTexCoord2f( 1.0f, 0.0f ); glVertex3f( vecMax[0], vecMax[1], vecMax[2] );
  glEnd( );

  //au-dessous
  glBindTexture( GL_TEXTURE_2D, texIDs[SKY_BOTTOM] );
  glBegin( GL_TRIANGLE_FAN );
  glTexCoord2f( 0.0f, 0.0f ); glVertex3f( vecMin[0], vecMin[1], vecMin[2] );
  glTexCoord2f( 0.0f, 1.0f ); glVertex3f( vecMin[0], vecMin[1], vecMax[2] );
  glTexCoord2f( 1.0f, 1.0f ); glVertex3f( vecMax[0], vecMin[1], vecMax[2] );
  glTexCoord2f( 1.0f, 0.0f ); glVertex3f( vecMax[0], vecMin[1], vecMin[2] );
  glEnd( );
  glPopMatrix( );
  //glDepthMask( GL_TRUE );
  //glEnable( GL_DEPTH_TEST );
  //glEnable( GL_CULL_FACE );
}

#include <qgl.h>
#include <stdio.h>

#define QT_LR_NODE 0
#define QT_LL_NODE 1
#define QT_UL_NODE 2
#define QT_UR_NODE 3


//codes decrivant le type de suite de triangles a creer, selon la presence d'enfants recursives
#define QT_COMPLETE_FAN 0
#define QT_LL_UR	5
#define QT_LR_UL	10
#define QT_NO_FAN       15

#define VIEW_RIGHT  0
#define VIEW_LEFT   1
#define VIEW_BOTTOM 2
#define VIEW_TOP    3
#define VIEW_FAR    4
#define VIEW_NEAR   5



struct SQT_VERTEX
{
  float height;
};


class QUADTREE : public TERRAIN
{
private:
  //matrice quadtree
  unsigned char* quadMatrix;

  //matrice decrivant la region vue par la camera
  float viewMatrix[6][4];

  float pX,pY,pZ;	//camera position; parametres de SimpleViewer-Object

  //niveau de detail
  float detailLevel;	//souhaite
  float minResolution;	//minimum

  void PropagateRoughness( void );
  void RefineNode( float x, float z, int edgeLength );
  void RenderNode( float x, float z, int edgeLength, bool multiTextures= true, bool detail= true );
  void RenderVertex( float x, float z, float u, float v, bool multiTextures );

  inline int GetMatrixIndex( int X, int Z )
  {	return ( ( Z*sizeHeightMap )+X );	}

public:

  bool Init( void );
  void Shutdown( void );

  void Update(float x, float y, float z );
  void Render( void );

  void ComputeView( void );
  bool CubeViewTest( float x, float y, float z, float size );

  inline void setCameraPosition(float x, float y, float z)
  {        	pX=x; pY=y; pZ=z;	}

  inline void SetDetailLevel( float detail )
  {	detailLevel= detail;	}


  inline void SetMinResolution( float res )
  {	minResolution= res;	}

  inline unsigned char GetQuadMatrixData( int X, int Z )
  {
    if ((X>sizeHeightMap) || (X<0) || (Z>sizeHeightMap) || (Z<0))
      {
    printf("Matrix limits exceeded: %d,%d\n",X,Z);
    return 255;
      }
    return quadMatrix[ ( Z*sizeHeightMap )+X];
  }

  QUADTREE( void )
  {
    detailLevel=	2.5f;	//50.0f;
    minResolution = 1.2f;	//10.0f;
  }

  ~QUADTREE( void )
  {	}
};
#define SQR( number )		( number*number )
#define CUBE( number )		( number*number*number )
#define MIN( a, b )  ( ( ( a )<( b ) )?( a ):( b ) )
#define MAX( a, b )  ( ( ( a )<( b ) )?( b ):( a ) )

//codes determinant le type de suite partielle de triangles a creer, selon la position dans le quadtree
static char suiteCode[] = { 10, 8, 8, 12, 8, 0, 12, 14, 8, 12, 0, 14, 12, 14, 14, 0 };
static char suiteStart[]= { 3,  3, 0,  3, 1, 0,  0,  3, 2,  2, 0,  2,  1,  1,  0, 0 };

static bool debugger = false;

bool QUADTREE::Init( void )
{
  int x, z;

  quadMatrix= new unsigned char [SQR( sizeHeightMap )];

  for( z=0; z<sizeHeightMap; z++ )
    {
      for( x=0; x<sizeHeightMap; x++ )
    {
      quadMatrix[GetMatrixIndex( x, z )]= 1;
    }
    }

  //repartir les triangles pour que les regions detailles/moins lisses obtiennent plus de triangles
  PropagateRoughness( );
  return true;
}

void QUADTREE::Shutdown( void )
{
  if( quadMatrix )
    delete[] quadMatrix;
}

void QUADTREE::Update( float x, float y, float z )
{
  float center;
  setCameraPosition(x,y,z);
  //centre de la carte
  center= ( sizeHeightMap )/2.0f;

  //reconstruire le "grid" par traversee top-down du quadtree
  RefineNode( center, center, sizeHeightMap );
}

//rendering d'un seul vertex
//params u,v deja normalises par rapport a sizeHeightMap
void QUADTREE::RenderVertex( float x, float z, float u, float v, bool multiTextures )
{
  unsigned char color= GetBrightnessAtPoint( ( int )x, ( int )z );
  if (debugger) printf("RenderVertex %f,%f - %f,%f\n",x,z,u,v);
  if (paintLighting)
    {
      glColor3ub( ( unsigned char )( color*rLight ),
          ( unsigned char )( color*gLight ),
          ( unsigned char )( color*bLight ) );
    }
  else
    glColor3ub( 255, 255, 255 );
  glMultiTexCoord2f( GL_TEXTURE0, u, v );
  if( multiTextures )
    glMultiTexCoord2f(GL_TEXTURE1,u*repeatDetailMap,
              v*repeatDetailMap );
  glVertex3f( x/sizeHeightMap*scaleSize,
          GetScaledHeightAtPoint( ( int )x, ( int )z )/sizeHeightMap,
          z/sizeHeightMap *scaleSize );
}



//debut du rendering recursif
void QUADTREE::Render( void )
{
  float center;
  if (debugger) printf("Render\n");
  //centre de la carte
  center= ( sizeHeightMap )/2.0f;

  //on fait le culling a travers le quadtree, pas avec le hardware
  glDisable( GL_CULL_FACE );

  //on a multitextures et on souhaite les afficher
  if( haveMultitexture && paintTextures )
    {
      glDisable( GL_BLEND );	//pas de combinaison de couleurs avec MULTITEXTURES,
      //.. la combi des deux textures se fait automatiquement en hardware

      //selectionner comme premiere unite de texture la texture de base (couleur selon hauteur)
      glActiveTexture( GL_TEXTURE0 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, textureColorID );

      //selectionner commer deuxieme unite de texture la texture de detail (structure)
      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, textureDetailID );

      //definir la maniere dont les couleurs des triangles sont crees en fct. des couleurs des textures
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );		//mode de combination des deux
      glTexEnvi( GL_TEXTURE_ENV, GL_RGB_SCALE, 2 );		//augmenter la luminosite des couleurs

      //render a partir du noeud principal au milieu de la carte (recursif a partir d'ici)
      RenderNode( center, center, sizeHeightMap, true, true );
    }

  //on a pas de multitextures mais on souhaite des textures
  else if( paintTextures )
    {
      //PREMIER PARCOURS: couleurs
      //selectionner comme unite de texture la texture de base (couleur selon hauteur)
      glActiveTexture( GL_TEXTURE0 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, textureColorID );

      RenderNode( center, center, sizeHeightMap, false, false );

      //DEUXIEME PARCOURS: DETAIL
      //preparer la texture de detail
      glActiveTexture( GL_TEXTURE0 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, textureDetailID );

      //activer la combinaison entre couleur existant (premier parcours) et couleur ajoutee (detail)
      glEnable( GL_BLEND );
      glBlendFunc( GL_ZERO, GL_SRC_COLOR );

      RenderNode( center, center, sizeHeightMap, false, true );

      glDisable( GL_BLEND );
    }

  else	//pas de textures du tout
    {
      RenderNode( center, center, sizeHeightMap, false, false );
      //sinon rendering brute force?
    }

  //liberer la deuxieme texture
  glActiveTexture( GL_TEXTURE1 );
  glDisable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, 0 );

  //liberer la premiere texture
  glActiveTexture( GL_TEXTURE0 );
  glDisable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, 0 );
}


//gerer la repartition des triangles sur les parties moins lisses / plus detaillees
void QUADTREE::PropagateRoughness( void )
{
  float upperBound;
  int dH, d2, localD2, localH;
  int edgeLength, edgeOffset;
  int childOffset;
  int x, z;
  if (debugger) printf("PropagateRoughness\n");
  //commencer au niveau de detail le plus elevee (taille de bloc la plus petite)
  edgeLength= 2;
  //jusqu'au niveau de detail le moins elevee (taille de bloc la plus petite)
  //un bloc est defini par edgeLength*edgeLength
  //.. ==> bottom-up!
  while( edgeLength<=sizeHeightMap )
    {
      edgeOffset= ( edgeLength )>>1;

      childOffset = ( edgeLength )>>2;
      //D2: valeur qui indique la difference maximale entre l'hauteur reelle et
      //.. l'hauteur moyenne autour du point (calculee comme moyenne de deux points)
      //pour chaque noeud, calculer D2 a 9 endroits (milieu + 8 autour, sur un rectangle) pour 5 valeurs
      for( z=edgeOffset; z<sizeHeightMap-edgeOffset; z+=( edgeLength ) )
    {
      for( x=edgeOffset; x<sizeHeightMap-edgeOffset; x+=( edgeLength ) )
        {
          //haut, milieu
          localD2= (int) ceil( (int)( abs( ( ( GetTrueHeightAtPoint( x-edgeOffset, z+edgeOffset )+GetTrueHeightAtPoint( x+edgeOffset, z+edgeOffset ) )>>1 ) - GetTrueHeightAtPoint( x, z+edgeOffset ) ) ) );
          //a droite, milieu
          dH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x+edgeOffset, z+edgeOffset )+
                    GetTrueHeightAtPoint( x+edgeOffset, z-edgeOffset ) )>>1 ) - GetTrueHeightAtPoint( x+edgeOffset, z ) ) );
          localD2= MAX( localD2, dH );

          //en bas, milieu
          dH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x-edgeOffset, z-edgeOffset )+
                    GetTrueHeightAtPoint( x+edgeOffset, z-edgeOffset ) )>>1 ) - GetTrueHeightAtPoint( x, z-edgeOffset ) ) );
          localD2= MAX( localD2, dH );

          //a gauche, milieu
          dH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x-edgeOffset, z+edgeOffset )+
                    GetTrueHeightAtPoint( x-edgeOffset, z-edgeOffset ) )>>1 ) - GetTrueHeightAtPoint( x-edgeOffset, z ) ) );
          localD2= MAX( localD2, dH );

          //deux points sur la diagonale de bas gauche a haut droite
          dH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x-edgeOffset, z-edgeOffset )+
                    GetTrueHeightAtPoint( x+edgeOffset, z+edgeOffset ) )>>1 ) - GetTrueHeightAtPoint( x, z ) ) );
          localD2= MAX( localD2, dH );

          //deux points sur la diagonale de bas droite a haut gauche
          dH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x+edgeOffset, z-edgeOffset )+
                    GetTrueHeightAtPoint( x-edgeOffset, z+edgeOffset ) )>>1 ) - GetTrueHeightAtPoint( x, z ) ) );
          localD2= MAX( localD2, dH );
          // localD2 de 0 a 255, normaliser par la taille du bloc actuelle (de 3 a sizeHeightMap)
          // ... donc multiplier par 3 pour obtenir une precision maximale de 0 a 255 pour d2
          localD2= ( int )ceil( ( localD2*3.0f )/edgeLength );
          //sur le niveau le plus bas, on calcule l'hauteur reelle maximale a la main
          if( edgeLength==2 )
        {
          d2= localD2;
          //haut, droite
          localH= GetTrueHeightAtPoint( x+edgeOffset, z+edgeOffset );
          //milieu, droite
          localH= MAX( localH, GetTrueHeightAtPoint( x+edgeOffset, z ) );
          //bas, droite
          localH= MAX( localH, GetTrueHeightAtPoint( x+edgeOffset, z-edgeOffset ) );
          //bas milieu
          localH= MAX( localH, GetTrueHeightAtPoint( x, z-edgeOffset ) );
          //bas gauche
          localH= MAX( localH, GetTrueHeightAtPoint( x-edgeOffset, z-edgeOffset ) );
          //gauche milieu
          localH= MAX( localH, GetTrueHeightAtPoint( x-edgeOffset, z ) );
          //gauche haut
          localH= MAX( localH, GetTrueHeightAtPoint( x-edgeOffset, z+edgeOffset ) );
          //haut milieu
          localH= MAX( localH, GetTrueHeightAtPoint( x, z+edgeOffset ) );
          //centre
          localH= MAX( localH, GetTrueHeightAtPoint( x, z ) );
          //sauvegarder ce valeur dans la matrice (pour les pas recursifs suivants)
          quadMatrix[GetMatrixIndex( x+1, z )]= localH;
        }
          //sur les autres niveaux, on utilise la valeur sauvegarde dans la matrice
          else
        {
          upperBound= 1.0f*minResolution/( 2.0f*( minResolution-1.0f ) );

          d2= (int)ceil(MAX(upperBound*(float)GetQuadMatrixData( x,z ),(float)localD2 ) );
          d2= (int)ceil(MAX(upperBound*(float)GetQuadMatrixData(x-edgeOffset,z),(float)d2));
          d2= (int)ceil(MAX(upperBound*(float)GetQuadMatrixData(x+edgeOffset,z),(float)d2));
          d2= (int)ceil(MAX(upperBound*(float)GetQuadMatrixData(x,z+edgeOffset),(float)d2));
          d2= (int)ceil(MAX(upperBound*(float)GetQuadMatrixData(x,z-edgeOffset),(float)d2));

          localH= GetTrueHeightAtPoint( x+childOffset, z+childOffset );
          localH= MAX( localH, GetTrueHeightAtPoint( x+childOffset, z-childOffset ) );
          localH= MAX( localH, GetTrueHeightAtPoint( x-childOffset, z-childOffset ) );
          localH= MAX( localH, GetTrueHeightAtPoint( x-childOffset, z+childOffset ) );

          quadMatrix[GetMatrixIndex( x+1, z )]= localH;
        }

          //sauvegarder les valeurs d2 dans la matrice
          quadMatrix[GetMatrixIndex( x, z )]  = d2;
          quadMatrix[GetMatrixIndex( x-1, z )]= d2;

          //la valeur se propage vers les noeuds voisins
          quadMatrix[GetMatrixIndex( x-edgeOffset, z-edgeOffset )]=
        MAX( GetQuadMatrixData( x-edgeOffset, z-edgeOffset ), d2 );
          quadMatrix[GetMatrixIndex( x-edgeOffset, z+edgeOffset )]=
        MAX( GetQuadMatrixData( x-edgeOffset, z+edgeOffset ), d2 );
          quadMatrix[GetMatrixIndex( x+edgeOffset, z+edgeOffset )]=
        MAX( GetQuadMatrixData( x+edgeOffset, z+edgeOffset ), d2 );
          quadMatrix[GetMatrixIndex( x+edgeOffset, z-edgeOffset )]=
        MAX( GetQuadMatrixData( x+edgeOffset, z-edgeOffset ), d2 );
        }
    }
      //monter un niveau (elargir la taille d'un bloc, reduire le niveau de detail)
      edgeLength= ( edgeLength<<1);
    }
}


// on determine la region capturee par la camera (en fct. de modelview et projection)
// idee pour le code: Chris Cookson, gamersdev.net
void QUADTREE::ComputeView( void )
{
  float projMatrix[16];	//projection matrice
  float modMatrix[16];	//modelview matrice
  float clip[16];
  float norm;
  if (debugger) printf("ComputeView\n");
  /* opengl: matrice de proj.*/
  glGetFloatv( GL_PROJECTION_MATRIX, projMatrix );

  /* opengl: matrice de vue*/
  glGetFloatv( GL_MODELVIEW_MATRIX, modMatrix );

  /* multiplication vue*proj*/
  clip[ 0]= modMatrix[ 0]*projMatrix[ 0] + modMatrix[ 1]*projMatrix[ 4] + modMatrix[ 2]*projMatrix[ 8] + modMatrix[ 3]*projMatrix[12];
  clip[ 1]= modMatrix[ 0]*projMatrix[ 1] + modMatrix[ 1]*projMatrix[ 5] + modMatrix[ 2]*projMatrix[ 9] + modMatrix[ 3]*projMatrix[13];
  clip[ 2]= modMatrix[ 0]*projMatrix[ 2] + modMatrix[ 1]*projMatrix[ 6] + modMatrix[ 2]*projMatrix[10] + modMatrix[ 3]*projMatrix[14];
  clip[ 3]= modMatrix[ 0]*projMatrix[ 3] + modMatrix[ 1]*projMatrix[ 7] + modMatrix[ 2]*projMatrix[11] + modMatrix[ 3]*projMatrix[15];

  clip[ 4]= modMatrix[ 4]*projMatrix[ 0] + modMatrix[ 5]*projMatrix[ 4] + modMatrix[ 6]*projMatrix[ 8] + modMatrix[ 7]*projMatrix[12];
  clip[ 5]= modMatrix[ 4]*projMatrix[ 1] + modMatrix[ 5]*projMatrix[ 5] + modMatrix[ 6]*projMatrix[ 9] + modMatrix[ 7]*projMatrix[13];
  clip[ 6]= modMatrix[ 4]*projMatrix[ 2] + modMatrix[ 5]*projMatrix[ 6] + modMatrix[ 6]*projMatrix[10] + modMatrix[ 7]*projMatrix[14];
  clip[ 7]= modMatrix[ 4]*projMatrix[ 3] + modMatrix[ 5]*projMatrix[ 7] + modMatrix[ 6]*projMatrix[11] + modMatrix[ 7]*projMatrix[15];

  clip[ 8]= modMatrix[ 8]*projMatrix[ 0] + modMatrix[ 9]*projMatrix[ 4] + modMatrix[10]*projMatrix[ 8] + modMatrix[11]*projMatrix[12];
  clip[ 9]= modMatrix[ 8]*projMatrix[ 1] + modMatrix[ 9]*projMatrix[ 5] + modMatrix[10]*projMatrix[ 9] + modMatrix[11]*projMatrix[13];
  clip[10]= modMatrix[ 8]*projMatrix[ 2] + modMatrix[ 9]*projMatrix[ 6] + modMatrix[10]*projMatrix[10] + modMatrix[11]*projMatrix[14];
  clip[11]= modMatrix[ 8]*projMatrix[ 3] + modMatrix[ 9]*projMatrix[ 7] + modMatrix[10]*projMatrix[11] + modMatrix[11]*projMatrix[15];

  clip[12]= modMatrix[12]*projMatrix[ 0] + modMatrix[13]*projMatrix[ 4] + modMatrix[14]*projMatrix[ 8] + modMatrix[15]*projMatrix[12];
  clip[13]= modMatrix[12]*projMatrix[ 1] + modMatrix[13]*projMatrix[ 5] + modMatrix[14]*projMatrix[ 9] + modMatrix[15]*projMatrix[13];
  clip[14]= modMatrix[12]*projMatrix[ 2] + modMatrix[13]*projMatrix[ 6] + modMatrix[14]*projMatrix[10] + modMatrix[15]*projMatrix[14];
  clip[15]= modMatrix[12]*projMatrix[ 3] + modMatrix[13]*projMatrix[ 7] + modMatrix[14]*projMatrix[11] + modMatrix[15]*projMatrix[15];

  /* plan droite*/
  viewMatrix[VIEW_RIGHT][0]= clip[ 3] - clip[ 0];
  viewMatrix[VIEW_RIGHT][1]= clip[ 7] - clip[ 4];
  viewMatrix[VIEW_RIGHT][2]= clip[11] - clip[ 8];
  viewMatrix[VIEW_RIGHT][3]= clip[15] - clip[12];

  //normaliser
  norm= sqrtf( SQR( viewMatrix[VIEW_RIGHT][0] )+
           SQR( viewMatrix[VIEW_RIGHT][1] )+
           SQR( viewMatrix[VIEW_RIGHT][2] ) );
  viewMatrix[VIEW_RIGHT][0]/= norm;
  viewMatrix[VIEW_RIGHT][1]/= norm;
  viewMatrix[VIEW_RIGHT][2]/= norm;
  viewMatrix[VIEW_RIGHT][3]/= norm;

  /* plan gauche*/
  viewMatrix[VIEW_LEFT][0]= clip[ 3] + clip[ 0];
  viewMatrix[VIEW_LEFT][1]= clip[ 7] + clip[ 4];
  viewMatrix[VIEW_LEFT][2]= clip[11] + clip[ 8];
  viewMatrix[VIEW_LEFT][3]= clip[15] + clip[12];

  //normaliser
  norm= sqrtf( SQR( viewMatrix[VIEW_LEFT][0] )+
           SQR( viewMatrix[VIEW_LEFT][1] )+
           SQR( viewMatrix[VIEW_LEFT][2] ) );
  viewMatrix[VIEW_LEFT][0]/= norm;
  viewMatrix[VIEW_LEFT][1]/= norm;
  viewMatrix[VIEW_LEFT][2]/= norm;
  viewMatrix[VIEW_LEFT][3]/= norm;

  /* plan bas*/
  viewMatrix[VIEW_BOTTOM][0]= clip[ 3] + clip[ 1];
  viewMatrix[VIEW_BOTTOM][1]= clip[ 7] + clip[ 5];
  viewMatrix[VIEW_BOTTOM][2]= clip[11] + clip[ 9];
  viewMatrix[VIEW_BOTTOM][3]= clip[15] + clip[13];

  //normaliser
  norm= sqrtf( SQR( viewMatrix[VIEW_BOTTOM][0] )+
           SQR( viewMatrix[VIEW_BOTTOM][1] )+
           SQR( viewMatrix[VIEW_BOTTOM][2] ) );
  viewMatrix[VIEW_BOTTOM][0]/= norm;
  viewMatrix[VIEW_BOTTOM][1]/= norm;
  viewMatrix[VIEW_BOTTOM][2]/= norm;
  viewMatrix[VIEW_BOTTOM][3]/= norm;

  /* plan haut*/
  viewMatrix[VIEW_TOP][0]= clip[ 3] - clip[ 1];
  viewMatrix[VIEW_TOP][1]= clip[ 7] - clip[ 5];
  viewMatrix[VIEW_TOP][2]= clip[11] - clip[ 9];
  viewMatrix[VIEW_TOP][3]= clip[15] - clip[13];

  //normaliser
  norm= sqrtf( SQR( viewMatrix[VIEW_TOP][0] )+
           SQR( viewMatrix[VIEW_TOP][1] )+
           SQR( viewMatrix[VIEW_TOP][2] ) );
  viewMatrix[VIEW_TOP][0]/= norm;
  viewMatrix[VIEW_TOP][1]/= norm;
  viewMatrix[VIEW_TOP][2]/= norm;
  viewMatrix[VIEW_TOP][3]/= norm;

  /* plan loin*/
  viewMatrix[VIEW_FAR][0]= clip[ 3] - clip[ 2];
  viewMatrix[VIEW_FAR][1]= clip[ 7] - clip[ 6];
  viewMatrix[VIEW_FAR][2]= clip[11] - clip[10];
  viewMatrix[VIEW_FAR][3]= clip[15] - clip[14];

  //normaliser
  norm= sqrtf( SQR( viewMatrix[VIEW_FAR][0] )+
           SQR( viewMatrix[VIEW_FAR][1] )+
           SQR( viewMatrix[VIEW_FAR][2] ) );
  viewMatrix[VIEW_FAR][0]/= norm;
  viewMatrix[VIEW_FAR][1]/= norm;
  viewMatrix[VIEW_FAR][2]/= norm;
  viewMatrix[VIEW_FAR][3]/= norm;

  /* plan proche*/
  viewMatrix[VIEW_NEAR][0]= clip[ 3] + clip[ 2];
  viewMatrix[VIEW_NEAR][1]= clip[ 7] + clip[ 6];
  viewMatrix[VIEW_NEAR][2]= clip[11] + clip[10];
  viewMatrix[VIEW_NEAR][3]= clip[15] + clip[14];

  //normaliser
  norm= sqrtf( SQR( viewMatrix[VIEW_NEAR][0] )+
           SQR( viewMatrix[VIEW_NEAR][1] )+
           SQR( viewMatrix[VIEW_NEAR][2] ) );
  viewMatrix[VIEW_NEAR][0]/= norm;
  viewMatrix[VIEW_NEAR][1]/= norm;
  viewMatrix[VIEW_NEAR][2]/= norm;
  viewMatrix[VIEW_NEAR][3]/= norm;
}

//on teste si un cube est visible (entieremen/partiellement) par la camera
bool QUADTREE::CubeViewTest( float x, float y, float z, float size )
{
  int i;
  if (debugger) printf("CubeViewTest\n");
  //tester les six bords du cube contre l'intersections avec les plans de vue
  for( i=0; i<6; i++ )
    {
      if( viewMatrix[i][0] * ( x-size )+viewMatrix[i][1] *
      ( y-size )+viewMatrix[i][2] *
      ( z-size )+viewMatrix[i][3] > 0 )
    continue;
      if( viewMatrix[i][0] * ( x+size )+viewMatrix[i][1] *
      ( y-size )+viewMatrix[i][2] *
      ( z-size )+viewMatrix[i][3] > 0 )
    continue;
      if( viewMatrix[i][0] * ( x-size )+viewMatrix[i][1] *
      ( y+size )+viewMatrix[i][2] *
      ( z-size )+viewMatrix[i][3] > 0 )
    continue;
      if( viewMatrix[i][0] * ( x+size )+viewMatrix[i][1] *
      ( y+size )+viewMatrix[i][2] *
      ( z-size )+viewMatrix[i][3] > 0 )
    continue;
      if( viewMatrix[i][0] * ( x-size )+viewMatrix[i][1] *
      ( y-size )+viewMatrix[i][2] *
      ( z+size )+viewMatrix[i][3] > 0 )
    continue;
      if( viewMatrix[i][0] * ( x+size )+viewMatrix[i][1] *
      ( y-size )+viewMatrix[i][2] *
      ( z+size )+viewMatrix[i][3] > 0 )
    continue;
      if( viewMatrix[i][0] * ( x-size )+viewMatrix[i][1] *
      ( y+size )+viewMatrix[i][2] *
      ( z+size )+viewMatrix[i][3] > 0 )
    continue;
      if( viewMatrix[i][0] * ( x+size )+viewMatrix[i][1] *
      ( y+size )+viewMatrix[i][2] *
      ( z+size )+viewMatrix[i][3] > 0 )
    continue;

      return false;
    }
  return true;
}



//decider si on sous-divise un noeud (ameliorer l'affichage si pret de la camera)
void QUADTREE::RefineNode( float x, float z, int edgeLength )
{
  float viewDistance, f;
  float childOffset;
  int childEdgeLength;
  int blend;
  if (debugger) printf("RefineNode: %f,%f:%d\n",x,z,edgeLength);


  //tester les bords d'un cube contenant le vertex actuel contre l'intersection avec la vue
  if( !CubeViewTest( x *scaleSize /sizeHeightMap,
             GetScaledHeightAtPoint( ( int  )x, ( int )z )/sizeHeightMap,
             z *scaleSize /sizeHeightMap,
             edgeLength *scaleSize /sizeHeightMap ) )	//*2
    {
      //desactiver ce noeud et ne plus continuer la recursion
      quadMatrix[GetMatrixIndex( ( int )x, ( int )z )]= 0;
      return;
    }

  //calculer la distance entre le vertex et la camera, norme L1

  viewDistance= ( float )( fabs( pX-( x *scaleSize /sizeHeightMap ) )+
               fabs( pY-GetScaledHeightAtPoint( ( int  )x+1, ( int )z )/sizeHeightMap )+
               fabs(pZ-( z *scaleSize /sizeHeightMap ) ) );


  //f: valeur qui decide si on subdivise un noeud ou non (selon l'article de Stefan Röttger sur le quadtree algo)
  f= viewDistance/( ( float )edgeLength*minResolution*	/*/sizeHeightMap* */
            MAX( ( float )GetQuadMatrixData( ( int )x-1, ( int )z )/3*detailLevel, 1.0f ) );
  if (debugger) printf("f: %f\n",f);
  if( f<1.0f )
    blend= 255;
  else
    blend= 0;

  quadMatrix[GetMatrixIndex( ( int )x, ( int )z )]= blend;

  if( blend!=0 )
    {
      //si on a deja atteint le niveau de taille de noeud le plus petit: plus de sousdivision possible
      if( edgeLength<=2 )
    return;

      //sinon, continuer la recursion du quadtree
      else
    {
      childOffset    = ( float )( ( edgeLength )>>2);
      childEdgeLength= ( edgeLength )>>1;

      //effectuer du "refinement" recursivement
      //bas gauche
      RefineNode( x-childOffset, z-childOffset, childEdgeLength );
      //bas droite
      RefineNode( x+childOffset, z-childOffset, childEdgeLength );
      //haut gauche
      RefineNode( x-childOffset, z+childOffset, childEdgeLength );
      //haut droite
      RefineNode( x+childOffset, z+childOffset, childEdgeLength );
      return;
    }
    }
}

//rendering (pas final!) du noeud (feuille) a la position x,z
void QUADTREE::RenderNode( float x, float z, int edgeLength, bool multiTextures, bool detail )
{
  float texLeft, texBottom, midX, midZ, texRight, texTop;
  float childOffset;
  float edgeOffset;
  int start, code;
  int childEdgeLength;
  int ichildOffset;
//  int iedgeOffset;
  int adjOffset;
  int suiteLength;
  int suitePosition;
  int blend;
  int iX, iZ;

  iX= ( int )x;
  iZ= ( int )z;
  if (debugger) printf("RenderNode: %f,%f:%d\n",x,z,edgeLength);
  //noeud marque "desactive": ne pas render
  if( GetQuadMatrixData( iX, iZ )==0 )
    return;

  //offset: puissances de 2; la position dans le Quadtree
//  iedgeOffset= ( edgeLength)/2;
  edgeOffset= ( edgeLength )/2.0f;

  //offset vers voisins
  adjOffset= edgeLength; //-1;

  if( detail && !multiTextures )
    {
      //coord. des textures
      texLeft  = ( ( float )fabs( x-edgeOffset )/sizeHeightMap )*repeatDetailMap;
      texBottom= ( ( float )fabs( z-edgeOffset )/sizeHeightMap )*repeatDetailMap;
      texRight = ( ( float )fabs( x+edgeOffset )/sizeHeightMap)*repeatDetailMap;
      texTop	  = ( ( float )fabs( z+edgeOffset )/sizeHeightMap )*repeatDetailMap;

      midX= ( ( texLeft+texRight )/2.0f );
      midZ= ( ( texBottom+texTop )/2.0f );
    }

  else	//multitextures disponibles (un seul parcours) ou pas de detail souhaite
    {
      texLeft  = ( ( float )fabs( x-edgeOffset )/sizeHeightMap );
      texBottom= ( ( float )fabs( z-edgeOffset )/sizeHeightMap );
      texRight = ( ( float )fabs( x+edgeOffset )/sizeHeightMap );
      texTop	  = ( ( float )fabs( z+edgeOffset )/sizeHeightMap );

      midX= ( ( texLeft+texRight )/2.0f );
      midZ= ( ( texBottom+texTop )/2.0f );
    }

  blend= GetQuadMatrixData( iX, iZ );

  if( blend>0)
    {
      //noeud sur le niveau le plus petit
      if( edgeLength<=2 )
    {
      glBegin( GL_TRIANGLE_FAN );
      //vertex au milieu
      RenderVertex( x, z, midX, midZ, multiTextures );

      //..bas gauche
      RenderVertex( x-edgeOffset, z-edgeOffset, texLeft, texBottom, multiTextures );

      //bas milieu, on n'affiche pas si voisin moins detaille (eviter "CRACKS")
      if( ( ( iZ-adjOffset )<=0 ) || GetQuadMatrixData( iX, iZ-adjOffset )!=0 )
        {
          RenderVertex( x, z-edgeOffset, midX, texBottom, multiTextures );
        }

      //bas droite
      RenderVertex( x+edgeOffset, z-edgeOffset, texRight, texBottom, multiTextures );

      //milieu droite, on n'affiche pas si voisin moins detaille (eviter "CRACKS")
      if( ( ( iX+adjOffset )>=sizeHeightMap ) || GetQuadMatrixData( iX+adjOffset, iZ )!=0 )
        {
          RenderVertex( x+edgeOffset, z, texRight, midZ, multiTextures );
        }

      //haut droite
      RenderVertex( x+edgeOffset, z+edgeOffset, texRight, texTop, multiTextures );

      //haut milieu,on n'affiche pas si voisin moins detaille (eviter "CRACKS")
      if( ( ( iZ+adjOffset )>=sizeHeightMap ) || GetQuadMatrixData( iX, iZ+adjOffset )!=0 )
        {
          RenderVertex( x, z+edgeOffset, midX, texTop, multiTextures );
        }

      //haut gauche
      RenderVertex( x-edgeOffset, z+edgeOffset, texLeft, texTop, multiTextures );

      //gauche milieu, on n'affiche pas si voisin moins detaille (eviter "CRACKS")
      if( ( ( iX-adjOffset )<=0 ) || GetQuadMatrixData( iX-adjOffset, iZ )!=0 )
        {
          RenderVertex( x-edgeOffset, z, texLeft, midZ, multiTextures );
        }

      //bas gauche
      RenderVertex( x-edgeOffset, z-edgeOffset, texLeft, texBottom, multiTextures );
      glEnd( );
      return;
    }

      else	//sur les autres niveaux: recursion
    {
      //position dans le quadtree
      ichildOffset = (edgeLength)/4;
      childOffset = (float)ichildOffset;

      childEdgeLength= ( edgeLength )/2;

      //on calcule un code indiquant le type de suite de triangles a creer
      //.. en fonction des valeurs dans notre quadtree-matrice
      //haut droite
      code = ( GetQuadMatrixData( iX+ichildOffset, iZ+ichildOffset )!=0 )*8;

      //haut gauche
      code|= ( GetQuadMatrixData( iX-ichildOffset, iZ+ichildOffset )!=0 )*4;

      //bas gauche
      code|= ( GetQuadMatrixData( iX-ichildOffset, iZ-ichildOffset )!=0 )*2;

      //bas droite
      code|= ( GetQuadMatrixData( iX+ichildOffset, iZ-ichildOffset )!=0 );

      //pas d'affichage de ce noeud, car il est compose de 4 "enfants" qui seront affiches recursivement
      if( code==QT_NO_FAN )
        {
          //bas gauche
          RenderNode( x-childOffset, z-childOffset, childEdgeLength );
          //bas droite
          RenderNode( x+childOffset, z-childOffset, childEdgeLength );
          //haut gauche
          RenderNode( x-childOffset, z+childOffset, childEdgeLength );
          //haut droite
          RenderNode( x+childOffset, z+childOffset, childEdgeLength );
          return;
        }

      //a afficher: bas gauche, haut droite; les autres sont des enfants
      if( code==QT_LL_UR )
        {
          //suite de triangles en haut droite
          glBegin( GL_TRIANGLE_FAN );
          //milieu
          RenderVertex( x, z, midX, midZ, multiTextures );

          //droite milieu
          RenderVertex( x+edgeOffset, z, texRight, midZ, multiTextures );

          //haut droite
          RenderVertex( x+edgeOffset, z+edgeOffset, texRight, texTop, multiTextures );

          //haut milieu
          RenderVertex( x, z+edgeOffset, midX, texTop, multiTextures );
          glEnd( );

          //suite en bas gauche
          glBegin( GL_TRIANGLE_FAN );
          //milieu
          RenderVertex( x, z, midX, midZ, multiTextures );

          //gauche milieu
          RenderVertex( x-edgeOffset, z, texLeft, midZ, multiTextures );

          //bas gauche
          RenderVertex( x-edgeOffset, z-edgeOffset, texLeft, texBottom, multiTextures );

          //bas milieu
          RenderVertex( x, z-edgeOffset, midX, texBottom, multiTextures );
          glEnd( );

          //recursion haut gauche, bas droite
          RenderNode( x-childOffset, z+childOffset, childEdgeLength );
          RenderNode( x+childOffset, z-childOffset, childEdgeLength );
          return;

        }

      //a afficher: triangles bas-droite, haut-gauche; les autres: enfants
      if( code==QT_LR_UL )
        {
          //haut gauche
          glBegin( GL_TRIANGLE_FAN );
          //milieu
          RenderVertex( x, z, midX, midZ, multiTextures );

          //haut milieu
          RenderVertex( x, z+edgeOffset, midX, texTop, multiTextures );

          //haut gauche
          RenderVertex( x-edgeOffset, z+edgeOffset, texLeft, texTop, multiTextures );

          //milieu gauche
          RenderVertex( x-edgeOffset, z, texLeft, midZ, multiTextures );
          glEnd( );

          //bas droite
          glBegin( GL_TRIANGLE_FAN );
          //milieu
          RenderVertex( x, z, midX, midZ, multiTextures );

          //bas milieu
          RenderVertex( x, z-edgeOffset, midX, texBottom, multiTextures );

          //bas droite
          RenderVertex( x+edgeOffset, z-edgeOffset, texRight, texBottom, multiTextures );

          //droite milieu
          RenderVertex( x+edgeOffset, z, texRight, midZ, multiTextures );
          glEnd( );

          //recursion haut droite, bas gauche: enfants
          RenderNode( x+childOffset, z+childOffset, childEdgeLength );
          RenderNode( x-childOffset, z-childOffset, childEdgeLength );
          return;
        }

      //feuille: pas d'enfants, rendering complet a faire
      if( code==QT_COMPLETE_FAN )
        {
          glBegin( GL_TRIANGLE_FAN );
          //vertex au milieu
          RenderVertex( x, z, midX, midZ, multiTextures );

          //..bas gauche
          RenderVertex( x-edgeOffset, z-edgeOffset, texLeft, texBottom, multiTextures );

          //bas milieu, on n'affiche pas si voisin moins detaille (eviter "CRACKS")
          if( ( ( iZ-adjOffset )<=0 ) || GetQuadMatrixData( iX, iZ-adjOffset )!=0 )
        {
          RenderVertex( x, z-edgeOffset, midX, texBottom, multiTextures );
        }

          //bas droite
          RenderVertex( x+edgeOffset, z-edgeOffset, texRight, texBottom, multiTextures );

          //milieu droite, on n'affiche pas si voisin moins detaille (eviter "CRACKS")
          if( ( ( iX+adjOffset )>=sizeHeightMap ) || GetQuadMatrixData( iX+adjOffset, iZ )!=0 )
        {
          RenderVertex( x+edgeOffset, z, texRight, midZ, multiTextures );
        }

          //haut droite
          RenderVertex( x+edgeOffset, z+edgeOffset, texRight, texTop, multiTextures );

          //haut milieu,on n'affiche pas si voisin moins detaille (eviter "CRACKS")
          if( ( ( iZ+adjOffset )>=sizeHeightMap ) || GetQuadMatrixData( iX, iZ+adjOffset )!=0 )
        {
          RenderVertex( x, z+edgeOffset, midX, texTop, multiTextures );
        }

          //haut gauche
          RenderVertex( x-edgeOffset, z+edgeOffset, texLeft, texTop, multiTextures );

          //gauche milieu, on n'affiche pas si voisin moins detaille (eviter "CRACKS")
          if( ( ( iX-adjOffset )<=0 ) || GetQuadMatrixData( iX-adjOffset, iZ )!=0 )
        {
          RenderVertex( x-edgeOffset, z, texLeft, midZ, multiTextures );
        }

          //bas gauche
          RenderVertex( x-edgeOffset, z-edgeOffset, texLeft, texBottom, multiTextures );
          glEnd( );
          return;
        }

      //il reste les suites partielles a afficher; on utilise les codes pour faciliter le choix de triang.
      //cette partie est quasiment directement copie de l'article de Stefan Röttger;
      //.. les codes des suites partielles sont pas evidents.. (code binaire pour representer un quadtree)
      //suite partielle = repartition non-symmetrique entre enfants recursifs et feuilles
      start= suiteStart[code];
      suiteLength= 0;

      //l'indice du permier bit non-null du code indique la taille de la suite a creer
      while( !( ( ( long )suiteCode[code] )&( 1<<suiteLength ) ) && suiteLength<8 )
        suiteLength++;

      //rendering
      glBegin( GL_TRIANGLE_FAN );
      //milieu recursif
      RenderVertex( x, z, midX, midZ, multiTextures );

      for( suitePosition=suiteLength; suitePosition>0; suitePosition-- )
        {
          switch( start )
        {
          //bas droite
        case QT_LR_NODE:
          //bas milieu, ne pas afficher si voisin moins detaille
          if( ( ( iZ-adjOffset )<=0 ) ||
              GetQuadMatrixData( iX, iZ-adjOffset )!=0 ||
              suitePosition==suiteLength )
            {
              RenderVertex( x, z-edgeOffset, midX,
                    texBottom, multiTextures );
            }

          //bas droite
          RenderVertex( x+edgeOffset, z-edgeOffset, texRight,
                texBottom, multiTextures );

          //droit milieu
          if( suitePosition==1 )
            {
              RenderVertex( x+edgeOffset, z, texRight,
                    midZ, multiTextures );
            }
          break;

          //bas gauche
        case QT_LL_NODE:
          //gauche milieu, ne pas afficher si voisin moins detaille
          if( ( ( x-adjOffset )<=0 ) ||
              GetQuadMatrixData( iX-adjOffset, iZ )!=0 ||
              suitePosition==suiteLength )
            {
              RenderVertex( x-edgeOffset, z, texLeft, midZ,
                    multiTextures );
            }

          //bas gauche
          RenderVertex( x-edgeOffset, z-edgeOffset, texLeft,
                texBottom, multiTextures );

          //bas milieu
          if( suitePosition==1 )
            {
              RenderVertex( x, z-edgeOffset, midX, texBottom,
                    multiTextures );
            }
          break;

          //haut gauche
        case QT_UL_NODE:
          //haut milieu, ne pas afficher si voisin moins detaille
          if( ( ( iZ+adjOffset )>=sizeHeightMap ) ||
              GetQuadMatrixData( iX, iZ+adjOffset )!=0 ||
              suitePosition==suiteLength )
            {
              RenderVertex( x, z+edgeOffset, midX, texTop,
                    multiTextures );
            }

          //haut gauche
          RenderVertex( x-edgeOffset, z+edgeOffset, texLeft,
                texTop, multiTextures );

          //gauche milieu
          if( suitePosition==1 )
            {
              RenderVertex( x-edgeOffset, z, texLeft, midZ,
                    multiTextures );
            }
          break;

          //haut droite
        case QT_UR_NODE:
          //droit milieu, ne pas afficher si voisin moins detaille
          if( ( ( iX+adjOffset )>=sizeHeightMap ) ||
              GetQuadMatrixData( iX+adjOffset, iZ )!=0 ||
              suitePosition==suiteLength )
            {
              RenderVertex( x+edgeOffset, z, texRight, midZ,
                    multiTextures );
            }

          //haut droite
          RenderVertex( x+edgeOffset, z+edgeOffset, texRight, texTop,
                multiTextures );

          //haut milieu
          if( suitePosition==1 )
            {
              RenderVertex( x, z+edgeOffset, midX, texTop,
                    multiTextures );
            }
          break;
        }
          start--;
          start&= 3;
        }
      glEnd( );

      //pour les cas toujours pas traites, on continue la recursion
      for( suitePosition=( 4-suiteLength ); suitePosition>0; suitePosition-- )
        {
          switch( start )
        {
          //bas droite
        case QT_LR_NODE:
          RenderNode( x+childOffset, z-childOffset, childEdgeLength );
          break;

          //bas gauche
        case QT_LL_NODE:
          RenderNode( x-childOffset, z-childOffset, childEdgeLength );
          break;

          //haut gauche
        case QT_UL_NODE:
          RenderNode( x-childOffset, z+childOffset, childEdgeLength );
          break;

          //haut droite
        case QT_UR_NODE:
          RenderNode( x+childOffset, z+childOffset, childEdgeLength );
          break;
        }
          start--;
          start&= 3;
        }
      return;
    }
    }
}

bool CheckExtension( const QString& szExtensionName )
{
  unsigned int uiNextExtension;
  char*		 szSupExt= ( char* )glGetString( GL_EXTENSIONS );;
  char*		 cEndExtensions;

  //find the end of the extension list
  cEndExtensions= szSupExt+strlen( szSupExt );

  //search through the entire list
  while( szSupExt<cEndExtensions )
    {
      //find the next extension in the list
      uiNextExtension= strcspn( szSupExt, " " );

      //check the extension to the one given in the argument list
      if( szExtensionName==szSupExt )
    {
      //the extension is supported
      // printf( "Your video card supports extension: %s", szExtensionName );
      return true;
    }

      //move to the nexte extension in the list
      szSupExt+= ( uiNextExtension+1 );
    }

  //the extension is not supported
  QMessageBox::warning(NULL, "Viewer", QString("Your video card does not support extension: %1").arg(szExtensionName));
  return false;
}

class GLTexturedObject:public pi::gl::GL_Object
{
  public:
    GLTexturedObject(string texFile=""):objFile(texFile){}

    virtual void draw()
    {
        if(textures.size()==0)
        {
            // Load textures and vertexs
            if(!objFile.size()){cerr<<"No object name setted!\n";textures.push_back(-1);return;}
            string   objFilePath=pi::Path(objFile).getFolderName();

            ifstream ifs(objFile.c_str());
            if(!ifs.is_open()) {cerr<<"can't open file "<<objFile<<endl;textures.push_back(-1);return;}
            //parse object file
            {
                int texNum;
                string line;
                getline(ifs,line);
                {
                    stringstream sst(line);
                    sst>>texNum;
                }
                for(;texNum>0;texNum--)
                {
                    if(!getline(ifs,line))
                    {
                        cerr<<"No enough texture detected.\n";textures.push_back(-1);
                        return;
                    }
                    QImage image;
                    if (image.load(QString::fromStdString(objFilePath+"/"+line)))
                    {
                        image = QGLWidget::convertToGLFormat(image);
                        GLuint texID;
                        //construire les textures openGL
                        glGenTextures( 1, &texID );
                        glBindTexture( GL_TEXTURE_2D,texID);
                        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

                        //les deux commandes suivantes servent a elargir la texture pq. la surface soit remplie;
                        //.. clamp_to_edge = repeter la derniere ligne de la texture (marche bien ici)
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
                        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
                                      GL_RGBA, GL_UNSIGNED_BYTE, image.bits() );
                        textures.push_back(texID);
                    }
                    else
                    {
                        cerr<<"Can't open file "<<line<<endl;textures.push_back(-1);
                        return;
                    }
                }
                while(getline(ifs,line))
                {
                    stringstream sst(line);
                    float v;
                    for(int i=0;i<21;i++)
                    {sst>>v;vertexs.push_back(v);}
                }
                if(vertexs.size()%21!=0) vertexs.clear();
            }
        }

        if(vertexs.size())
        {
            glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
            glEnable( GL_TEXTURE_2D );
            glPushMatrix( );
//            glRotatef(90,1,0,0);
//            //on construit a partir de la position de la camera (ou du milieu...)
//            glTranslatef( vecCenter[0], vecCenter[1], vecCenter[2] );
            for(int i=0,iend=vertexs.size();i<iend;i+=21)
            {
                glBindTexture( GL_TEXTURE_2D, textures[vertexs[i]]);
                glBegin( GL_POLYGON );
                glTexCoord2f( vertexs[i+1], vertexs[i+2] ); glVertex3f( vertexs[i+3], vertexs[i+4],vertexs[i+5]);
                glTexCoord2f( vertexs[i+6], vertexs[i+7] ); glVertex3f( vertexs[i+8], vertexs[i+9],vertexs[i+10]);
                glTexCoord2f( vertexs[i+11], vertexs[i+12] ); glVertex3f( vertexs[i+13], vertexs[i+14],vertexs[i+15]);
                glTexCoord2f( vertexs[i+16], vertexs[i+17] ); glVertex3f( vertexs[i+18], vertexs[i+19],vertexs[i+20]);
                glEnd();
            }
            glPopMatrix( );
        }
    }

private:
    string   objFile;
    std::vector<int>    textures;
    std::vector<float>  vertexs;
};

class GLScenceImpl
{
public:
    GLScenceImpl()
        :texobj(svar.GetString("TextureObject.File","")),valid(false),initialized(false)
    {

    }

    void setViewPoint(double x,double y,double z)
    {
        v=qglviewer::Vec(x,y,z);
    }

    void draw()
    {
        if(!initialized)
        {
            string texPath=svar.GetString("TexturePath","data/texture");
            mySky.LoadTexture( SKY_FRONT, QString::fromStdString(texPath+"/skyfront.jpg"));
            mySky.LoadTexture( SKY_BACK, QString::fromStdString(texPath+"/skyback.jpg"));
            mySky.LoadTexture( SKY_RIGHT,QString::fromStdString(texPath+"/skyright.jpg"));
            mySky.LoadTexture( SKY_LEFT, QString::fromStdString(texPath+"/skyleft.jpg") );
            mySky.LoadTexture( SKY_TOP, QString::fromStdString(texPath+"/skytop.jpg"));
            mySky.LoadTexture( SKY_BOTTOM, QString::fromStdString(texPath+"/skybottom.jpg"));
            mySky.Set( -500,-500,-500, 1000.0f );

//            int mapSize= svar.GetDouble("MapSize",256);
//            double scaleFactor=svar.GetDouble("ScaleFactor",20);

//            myQuadtree.MakeTerrainFault(mapSize, 32, 25, 150, 10 ); //terrain initial plus lisse

//            myQuadtree.SetHeightScale( scaleFactor/4.0f );
//            myQuadtree.SetSizeScale(scaleFactor);

//            if (!myQuadtree.LoadTile( LOWEST,  QString::fromStdString(texPath+"/lowest.jpg")) ||
//                !myQuadtree.LoadTile( MEDIUM,  QString::fromStdString(texPath+"/medium.jpg" )) ||
//                !myQuadtree.LoadTile( HIGH,   QString::fromStdString(texPath+"/high.jpg") ) ||
//                !myQuadtree.LoadTile( HIGHEST, QString::fromStdString(texPath+"/highest.jpg") ))
//              printf("Base Texture load failed\n");

//            //lire la carte de detail
//            if (!myQuadtree.LoadDetailMap( QString::fromStdString(texPath+"/detailMap.jpg") ))
//              printf("Detail Texture load failed\n");

//            //creer la texture complete, la sauvegarder
//            myQuadtree.GenerateTextureMap( 2*mapSize );	//double precision de la carte d'hauteur

//            //creer la carte d'ombrages
//            myQuadtree.CalculateLighting( );

//            myQuadtree.DoTexturing(true);
//            myQuadtree.DoLighting(true);
//            myQuadtree.SetDetailLevel( 50.0f/(mapSize/3));
//            myQuadtree.SetMinResolution( 10.0f/(mapSize/3));
//            myQuadtree.SetLightColor(1.0f,1.0f,1.0f);

//            myQuadtree.ComputeView();
//            myQuadtree.Init();
//            myQuadtree.Update(10,10,10);
            valid=true;
            initialized=true;
        }

        if(valid)
        {
            mySky.Render( );
            texobj.draw();
//            myQuadtree.Render();


        }
    }

    qglviewer::Vec v;
    SKY            mySky;
    QUADTREE       myQuadtree;
    GLTexturedObject  texobj;
    bool           valid,initialized;
};



GLScence::GLScence()
    :impl(new GLScenceImpl)
{

}

void GLScence::draw()
{
    impl->draw();
}

void GLScence::setViewPoint(double x, double y, double z)
{
    impl->setViewPoint(x,y,z);
}
