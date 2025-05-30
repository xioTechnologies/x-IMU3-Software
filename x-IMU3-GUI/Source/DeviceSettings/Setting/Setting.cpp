#include "DeviceSettings/DeviceSettingsIds.h"
#include "DeviceSettings/DeviceSettingsItem.h"
#include "Setting.h"

Setting::Setting(const juce::ValueTree& tree_, DeviceSettingsItem* const parentIfExpandable_) : tree(tree_),
                                                                                                parentIfExpandable(parentIfExpandable_)
{
    addAndMakeVisible(name);
    addChildComponent(modifiedIcon);
    addChildComponent(warningIcon);

    name.setText(tree[DeviceSettingsIds::name]);

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
    warningIcon.setBounds(modifiedIcon.getBounds());

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

void Setting::mouseDown(const juce::MouseEvent&)
{
    if (parentIfExpandable != nullptr)
    {
        parentIfExpandable->setOpen(parentIfExpandable->isOpen() == false);
    }
}

bool Setting::isReadOnly() const
{
    return tree[DeviceSettingsIds::readOnly];
}

const juce::Rectangle<int>& Setting::getValueBounds() const
{
    return valueBounds;
}

std::optional<juce::var> Setting::getValue() const
{
    return tree.hasProperty(DeviceSettingsIds::value) ? tree.getProperty(DeviceSettingsIds::value) : std::optional<juce::var>();
}

void Setting::setValue(const juce::var& value)
{
    tree.setPropertyExcludingListener(this, DeviceSettingsIds::value, value, nullptr);
    tree.setProperty(DeviceSettingsIds::status, (int) Status::modified, nullptr);
    tree.setProperty(DeviceSettingsIds::statusTooltip, "Modified but Not Written to Device", nullptr);
}

void Setting::updateIcon()
{
    auto updateIcons = [&](auto status, auto tooltip)
    {
        modifiedIcon.setVisible(status == Status::modified);
        warningIcon.setVisible(status == Status::warning);
        modifiedIcon.setTooltip(tooltip);
        warningIcon.setTooltip(tooltip);
    };

    if (parentIfExpandable == nullptr)
    {
        updateIcons((Status) (int) tree[DeviceSettingsIds::status], tree[DeviceSettingsIds::statusTooltip]);
        return;
    }

    if (parentIfExpandable->isOpen())
    {
        modifiedIcon.setVisible(false);
        warningIcon.setVisible(false);
        return;
    }

    const std::function<juce::ValueTree(juce::ValueTree)> getStatus = [&](auto settings)
    {
        juce::ValueTree status = settings;
        for (const auto setting : settings)
        {
            if (getStatus(setting)[DeviceSettingsIds::status] > status[DeviceSettingsIds::status])
            {
                status = getStatus(setting);
            }
        }
        return status;
    };
    updateIcons((Status) (int) getStatus(tree)[DeviceSettingsIds::status], getStatus(tree)[DeviceSettingsIds::statusTooltip]);
}

void Setting::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (parentIfExpandable != nullptr)
    {
        if (property == DeviceSettingsIds::status)
        {
            updateIcon();
        }
        return;
    }

    if (treeWhosePropertyHasChanged == tree)
    {
        if (property == DeviceSettingsIds::value)
        {
            valueChanged();
        }
        else if (property == DeviceSettingsIds::status || property == DeviceSettingsIds::statusTooltip)
        {
            updateIcon();
        }
        else
        {
            jassertfalse;
        }
    }
}
