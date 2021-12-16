#include "../DeviceSettingsIDs.h"
#include "../DeviceSettingsItem.h"
#include "Setting.h"

Setting::Setting(const juce::ValueTree& tree_, DeviceSettingsItem* const parentIfExpandable_) : tree(tree_),
                                                                                                parentIfExpandable(parentIfExpandable_)
{
    addAndMakeVisible(name);
    addChildComponent(hintIcon);
    addChildComponent(warningIcon);

    name.setText(tree[DeviceSettingsIDs::displayName]);

    updateIcon();

    if (parentIfExpandable != nullptr)
    {
        parentIfExpandable->onOpennessChanged = [this]
        {
            updateIcon();
        };
    }

    tree.addListener(this);
}

Setting::~Setting()
{
    if (parentIfExpandable != nullptr)
    {
        parentIfExpandable->onOpennessChanged = nullptr;
    }
}

void Setting::resized()
{
    auto bounds = getLocalBounds();

    hintIcon.setBounds(bounds.removeFromRight(25).reduced(5));
    warningIcon.setBounds(hintIcon.getBounds());

    if (auto* treeview = findParentComponentOfClass<juce::TreeView>())
    {
        valueBounds = bounds.removeFromRight(juce::jmax(treeview->getWidth() / 3, treeview->getWidth() - 270));
    }
    else
    {
        jassertfalse;
    }

    valueBounds.removeFromRight(2);
    valueBounds.reduce(0, juce::roundToInt(rowMargin * 0.5));

    name.setBounds(bounds.reduced(1, 2));
}

bool Setting::isReadOnly() const
{
    return tree[DeviceSettingsIDs::readOnly];
}

const juce::Rectangle<int>& Setting::getValueBounds() const
{
    return valueBounds;
}

juce::var Setting::getValue() const
{
    return tree.getProperty(DeviceSettingsIDs::value);
}

void Setting::setValue(const juce::var& value)
{
    tree.setPropertyExcludingListener(this, DeviceSettingsIDs::value, value, nullptr);
    tree.setProperty(DeviceSettingsIDs::status, (int) Status::changed, nullptr);
}

void Setting::updateIcon()
{
    if (parentIfExpandable != nullptr)
    {
        if (parentIfExpandable->isOpen())
        {
            warningIcon.setVisible(false);
            hintIcon.setVisible(false);
            return;
        }

        const std::function<Status(juce::ValueTree)> getStatus = [&](auto settings)
        {
            auto result = Status::normal;
            for (auto setting : settings)
            {
                result = juce::jmax(result, (Status) (int) setting.getProperty(DeviceSettingsIDs::status), getStatus(setting));
                if (result == Status::failed)
                {
                    return result;
                }
            }
            return result;
        };
        const auto status = getStatus(tree);
        hintIcon.setVisible(status == Status::changed);
        warningIcon.setVisible(status == Status::failed);
        return;
    }

    hintIcon.setVisible((Status) (int) tree.getProperty(DeviceSettingsIDs::status) == Status::changed);
    warningIcon.setVisible((Status) (int) tree.getProperty(DeviceSettingsIDs::status) == Status::failed);
}

void Setting::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (parentIfExpandable != nullptr)
    {
        if (property == DeviceSettingsIDs::status)
        {
            updateIcon();
        }
        return;
    }

    if (treeWhosePropertyHasChanged == tree)
    {
        if (property == DeviceSettingsIDs::value)
        {
            valueChanged();
        }
        else if (property == DeviceSettingsIDs::status)
        {
            updateIcon();
        }
        else
        {
            jassertfalse;
        }
    }
}
