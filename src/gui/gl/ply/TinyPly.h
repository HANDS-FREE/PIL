#ifndef tinyply_h
#define tinyply_h

#include <vector>
#include <algorithm>
#include <string>
#include <stdint.h>
#include <map>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <memory>
#include <functional>

namespace pi
{

    struct DataCursor
    {
        void * vector;
        uint8_t * data;
        uint32_t offset;
        bool realloc = false;
    };

    class PlyProperty
    {

    public:

        enum class Type : uint8_t
        {
            INVALID,
            INT8,
            UINT8,
            INT16,
            UINT16,
            INT32,
            UINT32,
            FLOAT32,
            FLOAT64
        };

        PlyProperty(std::istream& is);
        PlyProperty(Type type, const std::string & name) : propertyType(type), isList(false), name(name) {}
        PlyProperty(Type list_type, Type prop_type, const std::string & name, int listCount) : listType(list_type), propertyType(prop_type), isList(true), listCount(listCount), name(name) {}

        Type listType, propertyType;
        bool isList;
        int listCount = 0;
        std::string name;

    private:

        void parse_internal(std::istream & is);

    };

    inline std::string make_key(const std::string & a, const std::string & b)
    {
        return (a + "-" + b);
    }

    template<typename T>
    void ply_cast(void * dest, const char * src)
    {
        *(static_cast<T *>(dest)) = *(reinterpret_cast<const T *>(src));
    }

    template<typename T>
    T ply_read_ascii(std::istream & is)
    {
        T data;
        is >> data;
        return data;
    }

    template<typename T>
    void ply_cast_ascii(void * dest, std::istream & is)
    {
        *(static_cast<T *>(dest)) = ply_read_ascii<T>(is);
    }

    struct PropertyInfo { int stride; std::string str; };
    static std::map<PlyProperty::Type, PropertyInfo> PropertyTable
    {
        {PlyProperty::Type::INT8,       {1, "char"}},
        {PlyProperty::Type::UINT8,      {1, "uchar"}},
        {PlyProperty::Type::INT16,      {2, "short"}},
        {PlyProperty::Type::UINT16,     {2, "ushort"}},
        {PlyProperty::Type::INT32,      {4, "int"}},
        {PlyProperty::Type::UINT32,     {4, "uint"}},
        {PlyProperty::Type::FLOAT32,    {4, "float"}},
        {PlyProperty::Type::FLOAT64,    {8, "double"}},
        {PlyProperty::Type::INVALID,    {0, "INVALID"}}
    };

    inline PlyProperty::Type property_type_from_string(const std::string & t)
    {
        if      (t == "int8"    || t == "char")     return PlyProperty::Type::INT8;
        else if (t == "uint8"   || t == "uchar")    return PlyProperty::Type::UINT8;
        else if (t == "int16"   || t == "short")    return PlyProperty::Type::INT16;
        else if (t == "uint16"  || t == "ushort")   return PlyProperty::Type::UINT16;
        else if (t == "int32"   || t == "int")      return PlyProperty::Type::INT32;
        else if (t == "uint32"  || t == "uint")     return PlyProperty::Type::UINT32;
        else if (t == "float32" || t == "float")    return PlyProperty::Type::FLOAT32;
        else if (t == "float64" || t == "double")   return PlyProperty::Type::FLOAT64;
        return PlyProperty::Type::INVALID;
    }

    template<typename T>
    inline uint8_t * resize(void * v, int32_t newSize)
    {
        auto vec = static_cast<std::vector<T> *>(v);
        vec->resize(newSize);
        return reinterpret_cast<uint8_t *>(vec->data());
    }

    inline void resize_vector(const PlyProperty::Type t, void * v, int32_t newSize, uint8_t *& ptr)
    {
        switch (t)
        {
            case PlyProperty::Type::INT8:       ptr = resize<int8_t>(v, newSize);   break;
            case PlyProperty::Type::UINT8:      ptr = resize<uint8_t>(v, newSize);  break;
            case PlyProperty::Type::INT16:      ptr = resize<int16_t>(v, newSize);  break;
            case PlyProperty::Type::UINT16:     ptr = resize<uint16_t>(v, newSize); break;
            case PlyProperty::Type::INT32:      ptr = resize<int32_t>(v, newSize);  break;
            case PlyProperty::Type::UINT32:     ptr = resize<uint32_t>(v, newSize); break;
            case PlyProperty::Type::FLOAT32:    ptr = resize<float>(v, newSize);    break;
            case PlyProperty::Type::FLOAT64:    ptr = resize<double>(v, newSize);   break;
            case PlyProperty::Type::INVALID:    throw std::invalid_argument("invalid ply property");
        }
    }

    template <typename T>
    inline PlyProperty::Type property_type_for_type(std::vector<T> & theType)
    {
        if      (std::is_same<T, int8_t>::value)     return PlyProperty::Type::INT8;
        else if (std::is_same<T, uint8_t>::value)    return PlyProperty::Type::UINT8;
        else if (std::is_same<T, int16_t>::value)    return PlyProperty::Type::INT16;
        else if (std::is_same<T, uint16_t>::value)   return PlyProperty::Type::UINT16;
        else if (std::is_same<T, int32_t>::value)    return PlyProperty::Type::INT32;
        else if (std::is_same<T, uint32_t>::value)   return PlyProperty::Type::UINT32;
        else if (std::is_same<T, float>::value)      return PlyProperty::Type::FLOAT32;
        else if (std::is_same<T, double>::value)     return PlyProperty::Type::FLOAT64;
        else return PlyProperty::Type::INVALID;
    }

    class PlyElement
    {

    public:

        PlyElement(std::istream & istream);
        PlyElement(const std::string & name, int count) : name(name), size(count) {}
        std::string name;
        int size;
        std::vector<PlyProperty> properties;

    private:

        void parse_internal(std::istream & is);

    };

    inline int find_element(const std::string key, std::vector<PlyElement> & list)
    {
        for (size_t i = 0; i < list.size(); ++i)
            if (list[i].name == key) return i;
        return -1;
    }

    class PlyFile
    {

    public:

        PlyFile() {}
        PlyFile(std::istream & is);

        void read(std::istream & is);
        void write(std::ostringstream & os, bool isBinary);

        std::vector<PlyElement> & get_elements() { return elements; }

        std::vector<std::string> comments;
        std::vector<std::string> objInfo;

        template<typename T>
        int request_properties_from_element(std::string elementKey, std::vector<std::string> propertyKeys, std::vector<T> & source, int listCount = 1)
        {
            if (get_elements().size() == 0)
                return 0;

            if (find_element(elementKey, get_elements()) >= 0)
            {
                if (std::find(requestedElements.begin(), requestedElements.end(), elementKey) == requestedElements.end())
                    requestedElements.push_back(elementKey);
            }
            else return 0;

            // count and verify large enough
            auto instance_counter = [&](const std::string & elementKey, const std::string & propertyKey)
            {
                for (auto e : get_elements())
                {
                    if (e.name != elementKey) continue;
                    for (auto p : e.properties)
                    {
                        if (p.name == propertyKey)
                        {
                            if (PropertyTable[property_type_for_type(source)].stride != PropertyTable[p.propertyType].stride)
                                throw std::runtime_error("destination vector is wrongly typed to hold this property");
                            return e.size;

                        }
                    }
                }
                return 0;
            };

            // Properties in the userDataTable share the same cursor
            auto cursor = std::make_shared<DataCursor>();

            std::vector<uint32_t> instanceCounts;

            for (auto key : propertyKeys)
            {
                if (int instanceCount = instance_counter(elementKey, key))
                {
                    instanceCounts.push_back(instanceCount);
                    auto result = userDataTable.insert(std::pair<std::string, std::shared_ptr<DataCursor>>(make_key(elementKey, key), cursor));
                    if (result.second == false)
                        throw std::runtime_error("property has already been requested: " + key);
                }
                else return 0;
            }

            uint32_t totalInstanceSize = [&]() { uint32_t t = 0; for (auto c : instanceCounts) { t += c; } return t; }() * listCount;
            source.resize(totalInstanceSize); // this satisfies regular properties; cursor->realloc is for list types
            cursor->offset = 0;
            cursor->vector = &source;
            cursor->data = reinterpret_cast<uint8_t *>(source.data());

            if (listCount > 1)
            {
                cursor->realloc = true;
                return (totalInstanceSize / propertyKeys.size()) / listCount;
            }

            return totalInstanceSize / propertyKeys.size();
        }

        template<typename T>
        void add_properties_to_element(std::string elementKey, std::vector<std::string> propertyKeys, std::vector<T> & source, int listCount = 1, PlyProperty::Type listType = PlyProperty::Type::INVALID)
        {
            auto cursor = std::make_shared<DataCursor>();
            cursor->offset = 0;
            cursor->vector = &source;
            cursor->data = reinterpret_cast<uint8_t *>(source.data());

            auto create_property_on_element = [&](PlyElement & e)
            {
                for (auto key : propertyKeys)
                {
                    PlyProperty::Type t = property_type_for_type(source);
                    PlyProperty newProp = (listType == PlyProperty::Type::INVALID) ? PlyProperty(t, key) : PlyProperty(listType, t, key, listCount);
                    userDataTable.insert(std::pair<std::string, std::shared_ptr<DataCursor>>(make_key(e.name, key), cursor));
                    e.properties.push_back(newProp);
                }
            };

            int idx = find_element(elementKey, elements);
            if (idx >= 0)
            {
                PlyElement & e = elements[idx];
                create_property_on_element(e);
            }
            else
            {
                PlyElement newElement = listCount == 1 ? PlyElement(elementKey, (int) source.size() / (int) propertyKeys.size()) : PlyElement(elementKey, (int) source.size() / listCount);
                create_property_on_element(newElement);
                elements.push_back(newElement);
            }
        }

    private:

        uint32_t skip_property_binary(const PlyProperty & property, std::istream & is);
        void skip_property_ascii(const PlyProperty & property, std::istream & is);

        void read_property_binary(PlyProperty::Type t, void * dest, uint32_t & destOffset, std::istream & is);
        void read_property_ascii(PlyProperty::Type t, void * dest, uint32_t & destOffset, std::istream & is);
        void write_property_ascii(PlyProperty::Type t, std::ostringstream & os, uint8_t * src, uint32_t & srcOffset);
        void write_property_binary(PlyProperty::Type t, std::ostringstream & os, uint8_t * src, uint32_t & srcOffset);

        bool parse_header(std::istream & is);
        void write_header(std::ostringstream & os);

        void read_header_format(std::istream & is);
        void read_header_element(std::istream & is);
        void read_header_property(std::istream & is);
        void read_header_text(std::string line, std::istream & is, std::vector<std::string> place, int erase = 0);

        void read_internal(std::istream & is);

        void write_ascii_internal(std::ostringstream & os);
        void write_binary_internal(std::ostringstream & os);

        bool isBinary = false;
        bool isBigEndian = false;

        std::map<std::string, std::shared_ptr<DataCursor>> userDataTable;

        std::vector<PlyElement> elements;
        std::vector<std::string> requestedElements;
    };

} // tinyply

#endif // tinyply_h
