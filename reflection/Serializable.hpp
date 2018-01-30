#pragma once

#include <tuple>
#include <iostream>
#include <memory>
#include "meta/for_each.hpp"
#include "chop.hpp"
#include "fwd.hpp"
#include "OutputPolicies.hpp"

#include "reflection/Reflectible.hpp"
#include "meta/new_from_tuple.hpp"

namespace putils
{
    // OutputPolicy: type ressembling DefaultOutputPolicy (above), with a
    // template<T> static int serialize(std::ostream &s, const T &attr) function that will be called to
    // serialize each attribute
    template<typename Derived, bool Unserialize = true, typename OutputPolicy = OutputPolicies::Json>
    class Serializable
    {
        // Serialization implementation detail: pointer to generic tuple which will be serialized
        // Tuple should be given all the attributes to serialize
    private:
        // Base interface for Serializer template
        struct SerializerBase
        {
            virtual ~SerializerBase() {}

            virtual void serialize(const Derived *, std::ostream &s) const noexcept = 0;
            virtual void unserialize(Derived *, std::istream &s) = 0;
        };

        template<typename ...Attrs>
        struct Serializer : SerializerBase
        {
            Serializer(const std::tuple<Attrs...> &attrs)
            {
                if (!_first)
                    return;
                _first = false;

                _attrs = std::make_unique<std::tuple<Attrs...>>(attrs);
            }

            // For each member pointer in _attrs, serialize it
            void serialize(const Derived *obj, std::ostream &s) const noexcept final
            {
                OutputPolicy::startSerialize(s);

                bool first = true;
                pmeta::tuple_for_each(*_attrs, [&s, obj, &first](const auto &attr)
                {
                    const auto &val = obj->*(attr.second);

                    if (!first)
                        OutputPolicy::serializeNewField(s);
                    OutputPolicy::serialize(s, attr.first, val);
                    first = false;
                });

                OutputPolicy::endSerialize(s);
            }

            void unserializeImpl(putils::Serializable<Derived, true, OutputPolicy> *obj, std::istream &s)
            {
                OutputPolicy::unserialize(s, *static_cast<Derived*>(obj), *_attrs);
            }

            void unserializeImpl(putils::Serializable<Derived, false, OutputPolicy> *, std::istream &) noexcept
            {}

            void unserialize(Derived *obj, std::istream &s) override
            {
                unserializeImpl(obj, s);
            }

            // Static tuple containing the member pointers to be serialized for this class (Derived)
        private:
            static inline std::unique_ptr<std::tuple<Attrs ...>> _attrs = nullptr;
            static inline std::atomic<bool> _first = true;
        };

        template<typename ...Args>
        auto make_serializer(const std::tuple<Args...> &tuple)
        {
            return new Serializer<Args...>(tuple);
        }

        // Constructor
    public:
        template<typename ...Fields> // Fields: std::pair<std::string, Derived::*attr>
        Serializable(Fields &&...attrs)
        {
            if (!_serializer)
                _serializer.reset(new Serializer<Fields...>(std::tuple<Fields...>(FWD(attrs)...)));
        }

        // Reflectible constructor
    public:
        Serializable()
        {
            static_assert(
                    putils::is_reflectible<Derived>::value,
                    "Serializable types should be reflectible, or specify their fields at construction time"
            );

            if (!_serializer)
                _serializer.reset(make_serializer(Derived::get_attributes().getKeyValues()));
        }

    public:
        std::ostream &serialize(std::ostream &s) const noexcept
        {
            auto __tmp = static_cast<const Derived *>(this);
            _serializer->serialize(__tmp, s);
            return s;
        }

        std::istream &unserialize(std::istream &s)
        {
            _serializer->unserialize(static_cast<Derived *>(this), s);
            return s;
        }

    public:
        friend std::ostream &operator<<(std::ostream &s, const Derived &obj) noexcept
        {
            return obj.serialize(s);
        }

        friend std::istream &operator>>(std::istream &s, Derived &obj)
        {
            return obj.unserialize(s);
        }

    private:
        static inline std::unique_ptr<SerializerBase> _serializer = nullptr;
    };
}
