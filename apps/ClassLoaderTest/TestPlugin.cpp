#include "TestPlugin.h"
#include <base/ClassLoader/ClassLoader.h>
#include <base/Debug/Assert.h>
#include <assert.h>
#include <sstream>

using namespace pi;

void testClassLoader1()
{
    std::string path = "libTestLibrary";
    path.append(SharedLibrary::suffix());

    ClassLoader<TestPlugin> cl;

    assert (cl.begin() == cl.end());
    assert (cl.findClass("PluginA")==nullptr);
    assert (cl.findManifest(path)==nullptr);

    assert (!cl.isLibraryLoaded(path));

    try
    {
        const ClassLoader<TestPlugin>::Meta& meta = cl.classFor("PluginA");
        std::cout<<("not found - must throw exception");
    }
    catch (NotFoundException&)
    {
    }
    catch (...)
    {
        std::cout<<("wrong exception");
    }

    try
    {
        const ClassLoader<TestPlugin>::Manif& manif = cl.manifestFor(path);
        std::cout<<("not found - must throw exception");
    }
    catch (NotFoundException&)
    {
    }
    catch (...)
    {
        std::cout<<("wrong exception");
    }
}


void testClassLoader2()
{
    std::string path = "libTestLibrary";
    path.append(SharedLibrary::suffix());

    ClassLoader<TestPlugin> cl;
    cl.loadLibrary(path);

    assert (cl.begin() != cl.end());
    assert (cl.findClass("PluginA")==nullptr);
    assert (cl.findClass("PluginB")==nullptr);
    assert (cl.findClass("PluginC")==nullptr);
    assert (cl.findManifest(path)==nullptr);

    assert (cl.isLibraryLoaded(path));
    assert (cl.manifestFor(path).size() == 3);

    ClassLoader<TestPlugin>::Iterator it = cl.begin();
    assert (it != cl.end());
    assert (it->first == path);
    assert (it->second->size() == 3);
    ++it;
    assert (it == cl.end());

    TestPlugin* pPluginA = cl.classFor("PluginA").create();
    assert (pPluginA->name() == "PluginA");
    assert (!cl.classFor("PluginA").isAutoDelete(pPluginA));
    delete pPluginA;

    TestPlugin* pPluginB = cl.classFor("PluginB").create();
    assert (pPluginB->name() == "PluginB");
    delete pPluginB;

    pPluginB = cl.create("PluginB");
    assert (pPluginB->name() == "PluginB");
    delete pPluginB;

    assert (cl.canCreate("PluginA"));
    assert (cl.canCreate("PluginB"));
    assert (!cl.canCreate("PluginC"));

    TestPlugin& pluginC = cl.instance("PluginC");
    assert (pluginC.name() == "PluginC");

    try
    {
        TestPlugin& plgB = cl.instance("PluginB");
        std::cout<<("not a singleton - must throw");
    }
    catch (InvalidAccessException&)
    {
    }

    try
    {
        TestPlugin* pPluginC = cl.create("PluginC");
        std::cout<<("cannot create a singleton - must throw");
    }
    catch (InvalidAccessException&)
    {
    }

    try
    {
        const AbstractMetaObject<TestPlugin>& meta = cl.classFor("PluginC");
        meta.autoDelete(&(meta.instance()));
        std::cerr<<("cannot take ownership of a singleton - must throw");
    }
    catch (InvalidAccessException&)
    {
    }

    const AbstractMetaObject<TestPlugin>& meta1 = cl.classFor("PluginC");
    assert (meta1.isAutoDelete(&(meta1.instance())));

    // the following must not produce memory leaks
    const AbstractMetaObject<TestPlugin>& meta2 = cl.classFor("PluginA");
    meta2.autoDelete(meta2.create());
    meta2.autoDelete(meta2.create());

    TestPlugin* pPlugin = meta2.create();
    meta2.autoDelete(pPlugin);
    assert (meta2.isAutoDelete(pPlugin));
    meta2.destroy(pPlugin);
    assert (!meta2.isAutoDelete(pPlugin));

    cl.unloadLibrary(path);
}


void testClassLoader3()
{
    std::string path = "libTestLibrary";
    path.append(SharedLibrary::suffix());

    ClassLoader<TestPlugin> cl;
    cl.loadLibrary(path);
    cl.loadLibrary(path);
    cl.unloadLibrary(path);
    std::cout<<cl;

    assert (cl.manifestFor(path).size() == 3);

    ClassLoader<TestPlugin>::Iterator it = cl.begin();
    assert (it != cl.end());
    assert (it->first == path);
    assert (it->second->size() == 3);
    ++it;
    assert (it == cl.end());

    cl.unloadLibrary(path);
    assert (!cl.findManifest(path));
}

int main(int argc,char** argv)
{
    testClassLoader1();
    testClassLoader2();
    testClassLoader3();
    return 0;
}
