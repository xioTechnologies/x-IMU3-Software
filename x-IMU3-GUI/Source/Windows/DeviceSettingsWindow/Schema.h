#pragma once

#include <expected>
#include <juce_data_structures/juce_data_structures.h>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Ximu3.hpp"

namespace Schema {
    class Item {
    public:
        Item(const std::string &name);

        Item(const juce::ValueTree &tree);

        virtual ~Item() = default;

        virtual std::optional<std::string> getError() const = 0;

        std::string name{};
        std::string dependsOnKey{};
        std::vector<std::string> dependsOnValues{};
    };

    class Setting : public Item {
    public:
        enum class Type {
            string,
            number,
            boolean,
            enumeration,
        };

        enum class Status {
            unknown,
            confirmed,
            noResponse,
            errorResponse,
            invalidResponse,
        };

        std::string key{};
        Type type{};
        std::vector<std::pair<int, std::string> > enumeration{};
        bool readOnly{};

        std::string value{};
        std::string error{};
        Status status = Status::unknown;

        std::string emptyString{};

        Setting(const juce::ValueTree &tree, const juce::ValueTree &enums);

        Setting(const std::string &key_, const Type type_);

        std::optional<std::string> getError() const override;

        void clear();

        std::string getReadCommand() const;

        std::string getWriteCommand(const std::string &value_) const;

        void receive(const std::optional<ximu3::CommandMessage> &response);

        std::string getStringValue() const;

    private:
        static ximu3::XIMU3_JsonType jsonTypeFrom(const Type type_);
    };

    class Group : public Item {
    public:
        bool expand{};

        std::vector<std::unique_ptr<Item> > items{};

        Group(const juce::ValueTree &tree, const juce::ValueTree &enums);

        Group(std::vector<std::unique_ptr<Item> > settings);

        std::optional<std::string> getError() const override;

        std::vector<Setting *> flatten() const;

        Setting *find(const std::string &key) const;
    };

    std::unique_ptr<Group> loadSchema(const juce::ValueTree &tree);

    std::expected<std::unique_ptr<Group>, std::string> loadSchema(std::shared_ptr<ximu3::Connection>);
}
