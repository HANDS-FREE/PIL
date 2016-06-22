#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include <deque>

#include <base/Environment.h>
#include <base/Thread/Thread.h>
#include <base/Debug/Assert.h>

#include "UART.h"



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#if defined(PIL_OS_FAMILY_WINDOWS) || defined(PIL_MINGW)

#include <windows.h>

#define DEFAULT_READINTERVALTIMEOUT     50      /* Default read a char interval timeout in millseconds */
#define DEFAULT_READMULTIPLIER          50      /* Default read data timeout multiplier */
#define DEFAULT_READTOTALTIMEOUT        250     /* Default read total timeout */

#define DEFAULT_WRITEMULTIPLIER         50      /* Default write data timeout multiplier */
#define DEFAULT_WRITETOTALTIMEOUT       250     /* Default write total timeout */


namespace pi {

struct UART_inner_data
{
    HANDLE  m_hCom;                             /* Handle of COM Port */
    DCB     m_oDCB;                             /* COM Port Control Block */
};


UART::UART()
{
    UART_inner_data    *pd;

    // inner data
    pd = new UART_inner_data;
    pd->m_hCom = NULL;
    data = pd;

    // set default values
    port_name  = "COM1";
    baud_rate  = 115200;
    byte_size  = 8;
    parity_sw  = 1;
    stop_bits  = 1;

    timeout_sw = 0;
    DTR_sw     = 1;
    RTS_sw     = 1;
}


UART::~UART()
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->m_hCom != NULL ) {
        r = CloseHandle(pd->m_hCom);
        if( !r ) pi_dbg_error("Close port error!");
        pd->m_hCom = NULL;
    }

    delete pd;
    data = NULL;
}

int UART::open(const std::string &portName, int baudRate)
{
    UART_inner_data     *pd;

    char                szComPortName[200];
    COMMTIMEOUTS        oComTimeout;
    BOOL                fSuccess;

    int                 ret_code = 0;

    // handle structure
    pd = (UART_inner_data *) data;

    if( portName.size() ) port_name = portName;
    if( baudRate )        baud_rate = baudRate;

    // open port    
    pd->m_hCom = CreateFile(
                            port_name.c_str(),                  // COM port Name
                            GENERIC_READ | GENERIC_WRITE,       // read/write flags
                            0,                                  // comm devices must be opened w/exclusive-access
                            NULL,                               // no security attributes
                            OPEN_EXISTING,                      // COM devices must use OPEN_EXISTING
                            0,                                  // no overlapped I/O
                            NULL                                // hTemplate must be NULL for COM devices
                        );

    if (pd->m_hCom == INVALID_HANDLE_VALUE) {
        // Handle the error.
        pi_dbg_error("CreateFile failed with wrror %d.", GetLastError());
        return 1;
    }

    // We will build on the current configuration, and skip setting the size
    //      of the input and output buffers with SetupComm.
    fSuccess = GetCommState(pd->m_hCom, &(pd->m_oDCB));
    if ( !fSuccess ) {
        pi_dbg_error("UART::open GetComState failed with error %d.", GetLastError());
        ret_code = 2;
        goto UART_OPEN_ERR;
    }

    // Fill in the DCB
    switch(baud_rate) {
        case 110:
            pd->m_oDCB.BaudRate = CBR_110;
            break;
        case 300:
            pd->m_oDCB.BaudRate = CBR_300;
            break;
        case 600:
            pd->m_oDCB.BaudRate = CBR_600;
            break;
        case 1200:
            pd->m_oDCB.BaudRate = CBR_1200;
            break;
        case 2400:
            pd->m_oDCB.BaudRate = CBR_2400;
            break;
        case 4800:
            pd->m_oDCB.BaudRate = CBR_4800;
            break;
        case 9600:
            pd->m_oDCB.BaudRate = CBR_9600;
            break;
        case 14400:
            pd->m_oDCB.BaudRate = CBR_14400;
            break;
        case 19200:
            pd->m_oDCB.BaudRate = CBR_19200;
            break;
        case 38400:
            pd->m_oDCB.BaudRate = CBR_38400;
            break;
        case 56000:
            pd->m_oDCB.BaudRate = CBR_56000;
            break;
        case 57600:
            pd->m_oDCB.BaudRate = CBR_57600;
            break;
        case 115200:
            pd->m_oDCB.BaudRate = CBR_115200;
            break;
        case 128000:
            pd->m_oDCB.BaudRate = CBR_128000;
            break;
        case 2560000:
            pd->m_oDCB.BaudRate = CBR_256000;
            break;
        default:
            pi_dbg_error("UART::open Unsupport baud rate %d.", baud_rate);;
            ret_code = 3;
            goto UART_OPEN_ERR;
    }

    if( DTR_sw )
        pd->m_oDCB.fDtrControl = DTR_CONTROL_DISABLE;
    if( RTS_sw )
        pd->m_oDCB.fRtsControl = RTS_CONTROL_DISABLE;

    pd->m_oDCB.ByteSize = byte_size;
    pd->m_oDCB.Parity   = parity_sw;
    switch( stop_bits ) {
        case 1:
            pd->m_oDCB.StopBits = ONESTOPBIT;
            break;
        case 15:
            pd->m_oDCB.StopBits = ONE5STOPBITS;
            break;
        case 2:
            pd->m_oDCB.StopBits = TWOSTOPBITS;
            break;
    }

    fSuccess = SetCommState(pd->m_hCom, &(pd->m_oDCB));
    if (!fSuccess) {
        // Handle the error.
        pi_dbg_error("SetComState failed with error %d.", GetLastError());
        ret_code = 4;
        goto UART_OPEN_ERR;
    }

    // timout options
    if( timeout_sw ) {
        fSuccess = GetCommTimeouts(     pd->m_hCom,         /* Handle to comm device */
                                        &oComTimeout        /* time-out values */
                                  );
        if( !fSuccess ) {
            pi_dbg_error("GetCommTimeouts failed with error %d.", GetLastError());
            ret_code = 5;
            goto UART_OPEN_ERR;
        }

        oComTimeout.ReadIntervalTimeout         =  DEFAULT_READINTERVALTIMEOUT;
        oComTimeout.ReadTotalTimeoutMultiplier  =  DEFAULT_READMULTIPLIER;
        oComTimeout.ReadTotalTimeoutConstant    =  DEFAULT_READTOTALTIMEOUT;
        oComTimeout.WriteTotalTimeoutMultiplier =  DEFAULT_WRITEMULTIPLIER;
        oComTimeout.WriteTotalTimeoutConstant   =  DEFAULT_WRITETOTALTIMEOUT;

        fSuccess = SetCommTimeouts(     pd->m_hCom,
                                        &oComTimeout
                                  );
        if( !fSuccess ) {
            pi_dbg_error("UART::open SetCommTimeouts failed with error %d.", GetLastError());
            ret_code = 6;
            goto UART_OPEN_ERR;
        }
    }

    // port state
    goto UART_OPEN_RET;

UART_OPEN_ERR:
    CloseHandle(pd->m_hCom);
    pd->m_hCom = NULL;

UART_OPEN_RET:
    return ret_code;
}

int UART::close(void)
{
    UART_inner_data     *pd;
    int                 iRes;

    // handle structure
    pd = (UART_inner_data *) data;

    if( pd->m_hCom != NULL ) {
        iRes = CloseHandle(pd->m_hCom);
        if( !iRes ) {
            pi_dbg_error("Close COM port error!");
            return 2;
        }

        pd->m_hCom = NULL;
    } else {
        return 1;
    }

    return 0;
}

int UART::write(void *d, int len, int master)
{
    UART_inner_data     *pd;

    BOOL                bRes;
    unsigned long       iByteWritten;

    // handle structure
    pd = (UART_inner_data *) data;

    if( pd->m_hCom != NULL ) {
        // wirte data
        bRes = WriteFile(   pd->m_hCom,         /* Handle to file */
                            d,                  /* Data buffer */
                            len,                /* Number of bytes to write */
                            &iByteWritten,      /* Number of bytes written */
                            NULL                /* overlapped buffer */
                    );
        if( !bRes ) {
            pi_dbg_error("Send data error, errorcode: %d", GetLastError());
            return -2;
        }

        return iByteWritten;
    }

    return -1;
}

int UART::read(void *d, int len, int master)
{
    UART_inner_data     *pd;

    BOOL                bRes;
    int                 iByteRead = len;
    unsigned long       byte_read_act;

    // handle structure
    pd = (UART_inner_data *) data;

    // read data
    if( pd->m_hCom != NULL ) {
        bRes = ::ReadFile(    pd->m_hCom,         /* Handle to file */
                            d,                  /* Data buffer */
                            iByteRead,          /* Number of byte to read */
                            &byte_read_act,     /* Number of bytes readed */
                            NULL
                        );

        if( !bRes ) {
            pi_dbg_error("UART::read Read data error, errorcode: %d", GetLastError());
            return -2;
        }

        return byte_read_act;
    }

    return -1;
}


int UART::_write(void *d, int len)
{
    return 0;
}

int UART::_read(void *d,  int len)
{
    return 0;
}

} // end of namespace pi


#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#if PIL_OS_FAMILY_UNIX

#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {

inline int open_c(const char *__file, int __oflag, ...)
{
    return open(__file, __oflag);
}

} // end of extern "C"


namespace pi {

struct UART_inner_data
{
    int                 fd;

public:
    UART_inner_data() {
        fd = -1;
    }

    ~UART_inner_data() {
        if( fd != -1 ) ::close(fd);
        fd = -1;
    }
};

UART::UART()
{
    // inner data
    UART_inner_data *pd = new UART_inner_data;
    data = pd;

    // set default values
    port_name   = "/dev/ttyACM0";

    baud_rate   = 115200;
    byte_size   = 8;
    parity_sw   = 0;
    stop_bits   = 1;

    timeout_sw  = 0;
    DTR_sw      = 1;
    RTS_sw      = 1;

    m_options   = (UART_OPTIONS) (UART_NONBLOCK);
    m_bufSize   = 32;
}

UART::~UART()
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->fd >= 0 ) {
        r = ::close(pd->fd);
        if( r == -1 ) {
            pi_dbg_error("close port error!\n");
        }
    }

    delete pd;
    data = NULL;
}

//
// Reference:
//  http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
//  http://stackoverflow.com/questions/18108932/linux-c-serial-port-reading-writing
//  http://unixwiz.net/techtips/termios-vmin-vtime.html
//
int UART::open(const std::string &portName, int baudRate)
{
    UART_inner_data     *pd;
    int                 r = 0;

    pd = (UART_inner_data *) data;

    if( portName.size() ) port_name = portName;
    if( baudRate )        baud_rate = baudRate;

    //pd->fd = ::open(port_name, O_RDWR|O_NOCTTY|O_SYNC);
    //pd->fd = ::open(port_name, O_RDWR| O_NONBLOCK | O_NDELAY);

    if( m_options & UART_NONBLOCK )
        pd->fd = open_c(port_name.c_str(), O_RDWR|O_NOCTTY|O_SYNC|O_NONBLOCK);
    else
        pd->fd = open_c(port_name.c_str(), O_RDWR|O_NOCTTY|O_SYNC);

    if( pd->fd < 0 ) {
        pi_dbg_error("open port error! (%s)\n", port_name.c_str());
        return -1;
    }

    // set port baud rate
    speed_t b_speed;
    switch(baud_rate) {
    case 110:
        b_speed = B110;
        break;
    case 300:
        b_speed = B300;
        break;
    case 600:
        b_speed = B600;
        break;
    case 1200:
        b_speed = B1200;
        break;
    case 2400:
        b_speed = B2400;
        break;
    case 4800:
        b_speed = B4800;
        break;
    case 9600:
        b_speed = B9600;
        break;
    case 19200:
        b_speed = B19200;
        break;
    case 38400:
        b_speed = B38400;
        break;
    case 57600:
        b_speed = B57600;
        break;
    case 115200:
        b_speed = B115200;
        break;
    case 230400:
        b_speed = B230400;
        break;
    case 460800:
        b_speed = B460800;
        break;
    case 500000:
        b_speed = B500000;
        break;
    case 576000:
        b_speed = B576000;
        break;
    case 921600:
        b_speed = B921600;
        break;
    case 1000000:
        b_speed = B1000000;
        break;
    default:
        pi_dbg_error("unsupported baud rate: %d\n", baud_rate);
        r = -2;
        goto UART_OPEN_ERR;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if( tcgetattr(pd->fd, &tty) != 0 ) {
        pi_dbg_error("error %d from tcgetattr\n", errno);
        r = -3;
        goto UART_OPEN_ERR;
    }

    cfsetispeed(&tty, b_speed);
    cfsetospeed(&tty, b_speed);


    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    if( m_options & UART_NONBLOCK ) {
        tty.c_cc[VMIN]  =   0;              // read doesn't block
        tty.c_cc[VTIME] =   0;              // 0.5 seconds read timeout
    } else {
        tty.c_cc[VMIN]  =   1;              // read doesn't block
        tty.c_cc[VTIME] =   1;              // 0.1 seconds read timeout
    }

    /* Make raw */
    cfmakeraw(&tty);
    tcflush( pd->fd, TCIFLUSH );

    if (tcsetattr (pd->fd, TCSANOW, &tty) != 0) {
        pi_dbg_error("error %d from tcsetattr\n", errno);
        r = -4;
        goto UART_OPEN_ERR;
    }

    goto UART_OPEN_RET;

UART_OPEN_ERR:
    ::close(pd->fd);
    pd->fd = -1;

UART_OPEN_RET:
    return r;
}

int UART::close(void)
{
    UART_inner_data     *pd;
    int                 r=-1;

    pd = (UART_inner_data *) data;
    if( pd->fd >= 0 ) {
        r = ::close(pd->fd);
        if( r == -1 ) {
            pi_dbg_error("close port error!\n");
        }

        pd->fd = -1;
    }

    return r;
}


int UART::write(void *d, int len, int master)
{
    return _write(d, len);
}

int UART::read(void *d, int len, int master)
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->fd < 0 ) {
        pi_dbg_error("UART port not opened yet!\n");
        return -1;
    }


    r = _read(d, len);

    return r;
}


int UART::_write(void *d, int len)
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->fd >= 0 ) {
        r = ::write(pd->fd, d, len);
        return r;
    } else {
        pi_dbg_error("UART port not opened yet!\n");
        return -1;
    }
}

int UART::_read(void *d, int len)
{
    UART_inner_data *pd = (UART_inner_data*) data;

    return ::read(pd->fd, d, len);
}

} // end of namespace pi

#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


namespace pi {

class UART_InnerBuffer
{
public:
    UART_InnerBuffer() {
    }

    ~UART_InnerBuffer() {
        clear();
    }

    void clear(void) {
        readBuf.clear();
        writeBuf.clear();
    }

    int getReadBufSize(void) {
        pi::ScopedMutex m(muxRead);

        return readBuf.size();
    }

    int putReadBuf(uint8_t *d, int len) {
        pi::ScopedMutex m(muxRead);

        for(int i=0; i<len; i++) readBuf.push_back(d[i]);
        return len;
    }

    int getReadBuff(uint8_t *d, size_t len) {
        int l, i;
        pi::ScopedMutex m(muxRead);

        if( len <= readBuf.size() ) l = len;
        else l = readBuf.size();

        for(i=0; i<l; i++) {
            d[i] = readBuf.front();
            readBuf.pop_front();
        }

        return l;
    }

    int getWriteBufSize(void) {
        pi::ScopedMutex m(muxWrite);

        return writeBuf.size();
    }

    int putWriteBuf(uint8_t *d, int len) {
        pi::ScopedMutex m(muxWrite);

        for(int i=0; i<len; i++) writeBuf.push_back(d[i]);
        return len;
    }

    int getWriteBuff(uint8_t *d, size_t len) {
        int l, i;
        pi::ScopedMutex m(muxWrite);

        if( len <= writeBuf.size() ) l = len;
        else l = writeBuf.size();

        for(i=0; i<l; i++) {
            d[i] = writeBuf.front();
            writeBuf.pop_front();
        }

        return l;
    }

public:
    FILE                *fp;                        ///< reading from file

    pi::Mutex           muxRead, muxWrite;

    std::deque<uint8_t> readBuf;
    std::deque<uint8_t> writeBuf;
};



VirtualUART::VirtualUART() : m_uartType(VUART_FILE), m_buff(new UART_InnerBuffer)
{

}

VirtualUART::~VirtualUART()
{
    close();
}


int VirtualUART::open(const std::string &portName, int baudRate)
{
    if( portName.size() )   port_name = portName;
    if( baudRate )          baud_rate = baudRate;

    if( m_uartType == VUART_DEV ) {
        return UART::open(portName, baudRate);
    } else if( m_uartType == VUART_FILE ) {
        m_buff->fp = fopen(port_name.c_str(), "rw");
        if( m_buff->fp == NULL ) {
            pi_dbg_error("Can not open virtual UART: %s", port_name.c_str());
            return -1;
        }
    } else if ( m_uartType == VUART_IPC ) {
        m_buff->clear();
    }

    return 0;
}

int VirtualUART::close(void)
{
    if( m_uartType == VUART_DEV ) {
        return UART::close();
    } else if( m_uartType == VUART_FILE ) {
        if( m_buff->fp != NULL ) {
            fclose(m_buff->fp);
            m_buff->fp = NULL;
        }
    } else if ( m_uartType == VUART_IPC ) {
        m_buff->clear();
    }

    return 0;
}

int VirtualUART::write(void *d, int len, int master)
{
    if( m_uartType == VUART_DEV ) {
        return UART::write(d, len, master);
    } else if( m_uartType == VUART_FILE ) {
        if( m_buff->fp == NULL ) {
            pi_dbg_error("file not opend yet!");
            return -1;
        }

        return fwrite(d, 1, len, m_buff->fp);
    } else if( m_uartType == VUART_IPC ) {
        if( master ) return m_buff->putWriteBuf((uint8_t*)d, len);
        else         return m_buff->putReadBuf((uint8_t*)d, len);
    }
    return 0;
}

int VirtualUART::read(void *d,  int len, int master)
{
    if( m_uartType == VUART_DEV ) {
        return UART::read(d, len, master);
    } else if( m_uartType == VUART_FILE ) {
        if( m_buff->fp == NULL ) {
            pi_dbg_error("file not opend yet!");
            return -1;
        }

        if( feof(m_buff->fp) ) {
            return -1;
        }

        return fread(d, 1, len, m_buff->fp);
    } else {
        if( master ) return m_buff->getReadBuff((uint8_t*)d, len);
        else         return m_buff->getWriteBuff((uint8_t*)d, len);
    }
}


} // end of namespace pi

#ifdef PIL_OS_FAMILY_UNIX
#include <base/Path/Path_UNIX.h>
#endif

namespace pi {


#if defined(PIL_OS_FAMILY_WINDOWS) || defined(PIL_MINGW)

#include <windows.h>

StringArray UART_getDeviceList(void)
{
    StringArray devList;

    LPCTSTR regPath_COM = "HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
    HKEY    hKey;

    LONG  Status, ret0;
    DWORD dwIndex = 0;
    CHAR  Name[64];
    DWORD dwName;
    UCHAR szPortName[64];
    DWORD Type;
    DWORD dwSizeofPortName;

    // open the regist key, if success then return ERROR_SUCCESS=0
    ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath_COM, 0, KEY_READ, &hKey);

    if( ret0 == ERROR_SUCCESS ) {
        do {
            dwName = sizeof(Name);
            dwSizeofPortName = sizeof(szPortName);

            // read the contents
            Status = RegEnumValue(hKey, dwIndex++, Name, &dwName, NULL, &Type,
                                  szPortName, &dwSizeofPortName);
            if( (Status == ERROR_SUCCESS) || (Status == ERROR_MORE_DATA) ) {
                devList.push_back((char*) szPortName);
            }
        } while( (Status == ERROR_SUCCESS) || (Status == ERROR_MORE_DATA) );

        RegCloseKey(hKey);
    }

    return devList;
}


#endif


inline int stringContins(const std::string& s, const std::string& p)
{
    std::string::size_type idx = s.find(p);

    if( idx != std::string::npos )
        return 1;
    else
        return 0;
}

#ifdef PIL_OS_FAMILY_UNIX


StringArray UART_getDeviceList(void)
{
    std::string devPath = "/dev/";

    StringArray devList;
    StringArray allDev;

    // list all devices
    PathImpl::lsdir(devPath,allDev);

    // for each device
    StringArray::iterator it;
    for(it = allDev.begin(); it != allDev.end(); it++) {
        std::string& s = *it;

        if( stringContins(s, "tty") ) {
            if( stringContins(s, "USB") || stringContins(s, "ACM") ) {
                std::string devP = devPath+s;
                devList.push_back(devP);
            }
        }
    }

    return devList;
}

#endif

}
