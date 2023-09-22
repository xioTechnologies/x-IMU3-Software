#include "DeviceSettings/DeviceSettingsIDs.h"
#include "DeviceSettings/DeviceSettingsItem.h"
#include "Setting.h"

Setting::Setting(const juce::ValueTree& tree_, DeviceSettingsItem* const parentIfExpandable_) : tree(tree_),
                                                                                                parentIfExpandable(parentIfExpandable_)
{
    addAndMakeVisible(name);
    addChildComponent(modifiedIcon);
    addChildComponent(readFailedIcon);
    addChildComponent(writeFailedIcon);

    name.setText(tree[DeviceSettingsIDs::name]);

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

    modifiedIcon.setBounds(bounds.removeFromRight(25).reduced(5));
    readFailedIcon.setBounds(modifiedIcon.getBounds());
    writeFailedIcon.setBounds(modifiedIcon.getBounds());

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

std::optional<juce::var> Setting::getValue() const
{
    return tree.hasProperty(DeviceSettingsIDs::value) ? tree.getProperty(DeviceSettingsIDs::value) : std::optional<juce::var>();
}

void Setting::setValue(const juce::var& value)
{
    tree.setPropertyExcludingListener(this, DeviceSettingsIDs::value, value, nullptr);
    tree.setProperty(DeviceSettingsIDs::status, (int) Status::modified, nullptr);
}

void Setting::updateIcon()
{
    if (parentIfExpandable == nullptr)
    {
        modifiedIcon.setVisible((Status) (int) tree.getProperty(DeviceSettingsIDs::status) == Status::modified);
        readFailedIcon.setVisible((Status) (int) tree.getProperty(DeviceSettingsIDs::status) == Status::readFailed);
        writeFailedIcon.setVisible((Status) (int) tree.getProperty(DeviceSettingsIDs::status) == Status::writeFailed);
        return;
    }

    if (parentIfExpandable->isOpen())
    {
        modifiedIcon.setVisible(false);
        readFailedIcon.setVisible(false);
        writeFailedIcon.setVisible(false);
        return;
    }

    const std::function<Status(juce::ValueTree)> getStatus = [&](auto settings)
    {
        auto status = Status::normal;
        for (const auto setting : settings)
        {
            status = juce::jmax(status, (Status) (int) setting.getProperty(DeviceSettingsIDs::status), getStatus(setting));
        }
        return status;
    };
    const auto status = getStatus(tree);
    modifiedIcon.setVisible(status == Status::modified);
    readFailedIcon.setVisible(status == Status::readFailed);
    writeFailedIcon.setVisible(status == Status::writeFailed);
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
