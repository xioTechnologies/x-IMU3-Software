#include "ConnectionPanelContainer.h"
#include "ThreeDViewWindow.h"
#include "Widgets/PopupMenuHeader.h"

ThreeDViewWindow::ThreeDViewWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_, OpenGLRenderer &openGLRenderer)
    : Window(windowLayout_, type_, connectionPanel_, "3D View Menu"),
      threeDView(openGLRenderer) {
    addAndMakeVisible(threeDView);

    addAndMakeVisible(rollLabel);
    addAndMakeVisible(rollValue);
    addAndMakeVisible(pitchLabel);
    addAndMakeVisible(pitchValue);
    addAndMakeVisible(yawLabel);
    addAndMakeVisible(yawValue);
    addAndMakeVisible(ahrsStatusLabel);
    addAndMakeVisible(axesConventionLabel);
    addAndMakeVisible(infoLabel);

    quaternionCallbackId = connectionPanel.getConnection()->addQuaternionCallback(quaternionCallback = [&](auto message) {
        threeDView.update(message.x, message.y, message.z, message.w);

        const auto eulerAngles = ximu3::XIMU3_quaternion_message_to_euler_angles_message(message);

        roll = eulerAngles.roll;
        pitch = eulerAngles.pitch;
        yaw = eulerAngles.yaw;
    });

    rotationMatrixCallbackId = connectionPanel.getConnection()->addRotationMatrixCallback(rotationMatrixCallback = [&](auto message) {
        quaternionCallback(ximu3::XIMU3_euler_angles_message_to_quaternion_message(ximu3::XIMU3_rotation_matrix_message_to_euler_angles_message(message)));
    });

    eulerAnglesCallbackId = connectionPanel.getConnection()->addEulerAnglesCallback(eulerAnglesCallback = [&](auto message) {
        const auto quaternion = ximu3::XIMU3_euler_angles_message_to_quaternion_message(message);

        threeDView.update(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

        roll = message.roll;
        pitch = message.pitch;
        yaw = message.yaw;
    });

    ahrsStatusMessageCallbackId = connectionPanel.getConnection()->addAhrsStatusCallback(ahrsStatusMessageCallback = [&](ximu3::XIMU3_AhrsStatusMessage message) {
        juce::MessageManager::callAsync([&, self = SafePointer(this), message] {
            ahrsStatusLabel.setText(juce::String::createStringFromData(message.char_array, (int) message.number_of_bytes));
            resized();

            ahrsStatusLabelTimer.startTimer(5000);
        });
    });

    pingCallbackId = connectionPanel.getConnection()->addPingCallback(pingCallback = [&, model = std::optional<juce::String>()](auto response) mutable {
        if (model == juce::String(response.device_name)) {
            return;
        }

        model = response.device_name;

        juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this)] {
            if (self == nullptr) {
                return;
            }

            updateModel();
        });
    });

    threeDView.setSettings(readFromValueTree());
    updateModel();

    rollPitchYawTimer.startTimerHz(25);
}

ThreeDViewWindow::~ThreeDViewWindow() {
    connectionPanel.getConnection()->removeCallback(quaternionCallbackId);
    connectionPanel.getConnection()->removeCallback(rotationMatrixCallbackId);
    connectionPanel.getConnection()->removeCallback(eulerAnglesCallbackId);
    connectionPanel.getConnection()->removeCallback(ahrsStatusMessageCallbackId);
    connectionPanel.getConnection()->removeCallback(pingCallbackId);
}

void ThreeDViewWindow::resized() {
    Window::resized();
    juce::Rectangle<int> bounds = getContentBounds();

    compactView = (getWidth() < 350) || (getHeight() < 200);

    threeDView.setBounds(bounds);
    threeDView.setHudEnabled(compactView == false);

    updateEulerAnglesVisibilities();
    updateAhrsStatusVisibility();
    updateAxesConventionLabel();

    bounds.reduce(10, 10);

    ahrsStatusLabel.setBounds(bounds.reduced(100, 0));
    axesConventionLabel.setBounds(bounds);

    const auto setRow = [&](auto &label, auto &value) {
        auto row = bounds.removeFromTop(20);
        label.setBounds(row.removeFromLeft(50));
        value.setBounds(row);
    };

    setRow(rollLabel, rollValue);
    setRow(pitchLabel, pitchValue);
    setRow(yawLabel, yawValue);

    infoLabel.setBounds(bounds.removeFromBottom(20));
}

void ThreeDViewWindow::mouseDown(const juce::MouseEvent &mouseEvent) {
    lastMousePosition = mouseEvent.getPosition();

    if (mouseEvent.mods.isPopupMenu()) {
        getMenu().showMenuAsync({});
    }
}

void ThreeDViewWindow::mouseDrag(const juce::MouseEvent &mouseEvent) {
    if (connectionPanel.getConnectionPanelContainer().getCurrentlyShowingDragOverlay() != nullptr) {
        return;
    }

    const auto scale = 0.5f;

    auto settings = threeDView.getSettings();
    settings.cameraAzimuth = wrapAngle(settings.cameraAzimuth + (scale * (mouseEvent.getPosition().getX() - lastMousePosition.getX())));
    settings.cameraElevation = wrapAngle(settings.cameraElevation + (scale * (mouseEvent.getPosition().getY() - lastMousePosition.getY())));
    writeToValueTree(settings);

    lastMousePosition = mouseEvent.getPosition();
}

void ThreeDViewWindow::mouseDoubleClick(const juce::MouseEvent &) {
    auto settings = threeDView.getSettings();
    settings.cameraAzimuth = ThreeDView::Settings().cameraAzimuth;
    settings.cameraElevation = ThreeDView::Settings().cameraElevation;
    settings.cameraOrbitDistance = ThreeDView::Settings().cameraOrbitDistance;
    writeToValueTree(settings);
}

void ThreeDViewWindow::mouseWheelMove(const juce::MouseEvent &, const juce::MouseWheelDetails &wheel) {
    auto settings = threeDView.getSettings();
    settings.cameraOrbitDistance = juce::jlimit(1.0f, 4.0f, settings.cameraOrbitDistance + (0.5f * -wheel.deltaY));
    writeToValueTree(settings);
}

float ThreeDViewWindow::wrapAngle(float angle) {
    while (angle > 180.0f) {
        angle -= 360.0f;
    }

    while (angle < -180.0f) {
        angle += 360.0f;
    }

    return angle;
}

juce::String ThreeDViewWindow::toString(const ThreeDView::AxesConvention axesConvention) {
    switch (axesConvention) {
        case ThreeDView::AxesConvention::nwu:
            return "NWU";
        case ThreeDView::AxesConvention::enu:
            return "ENU";
        case ThreeDView::AxesConvention::ned:
            return "NED";
    }
    return "";
}

void ThreeDViewWindow::writeToValueTree(const ThreeDView::Settings &settings) {
    settingsTree.setProperty("cameraAzimuth", settings.cameraAzimuth, nullptr);
    settingsTree.setProperty("cameraElevation", settings.cameraElevation, nullptr);
    settingsTree.setProperty("cameraOrbitDistance", settings.cameraOrbitDistance, nullptr);
    settingsTree.setProperty("worldEnabled", settings.worldEnabled, nullptr);
    settingsTree.setProperty("modelEnabled", settings.modelEnabled, nullptr);
    settingsTree.setProperty("axesEnabled", settings.axesEnabled, nullptr);
    settingsTree.setProperty("compassEnabled", settings.compassEnabled, nullptr);
    settingsTree.setProperty("axesConvention", static_cast<int>(settings.axesConvention), nullptr);
}

ThreeDView::Settings ThreeDViewWindow::readFromValueTree() const {
    ThreeDView::Settings settings;
    settings.cameraAzimuth = settingsTree.getProperty("cameraAzimuth", settings.cameraAzimuth);
    settings.cameraElevation = settingsTree.getProperty("cameraElevation", settings.cameraElevation);
    settings.cameraOrbitDistance = settingsTree.getProperty("cameraOrbitDistance", settings.cameraOrbitDistance);
    settings.worldEnabled = settingsTree.getProperty("worldEnabled", settings.worldEnabled);
    settings.modelEnabled = settingsTree.getProperty("modelEnabled", settings.modelEnabled);
    settings.axesEnabled = settingsTree.getProperty("axesEnabled", settings.axesEnabled);
    settings.compassEnabled = settingsTree.getProperty("compassEnabled", settings.axesEnabled);
    settings.axesConvention = ThreeDView::axesConventionFrom(settingsTree.getProperty("axesConvention", static_cast<int>(settings.axesConvention)));
    return settings;
}

void ThreeDViewWindow::updateModel() {
    const auto showModel = [&] (const juce::File& file) {
        threeDView.setModel(file);
        infoLabel.setText("Loading...");
        loadingTimer.startTimerHz(5);
    };

    const auto showInfo = [&] (const juce::String& info) {
        threeDView.setModel({});
        infoLabel.setText(info);
        loadingTimer.stopTimer();
    };

    if (const auto file = Models::get(settingsTree["model"])) {
        currentModel = file->getFileName();
        showModel(*file);
        return;
    }

    currentModel = "auto";

    const auto response = connectionPanel.getConnection()->getPingResponse();

    if (response.has_value() == false) {
        showInfo("Waiting for ping response");
        return;
    }

    const juce::String model = response->device_name; // TODO: Use model

    const auto file = Models::find(model);

    if (file.has_value() == false) {
        showInfo("3D model not found for " + model);
        return;
    }

    showModel(*file);
}

void ThreeDViewWindow::updateEulerAnglesVisibilities() {
    const auto visible = (settingsTree.getProperty("eulerAnglesEnabled", true) && compactView == false);

    rollLabel.setVisible(visible);
    pitchLabel.setVisible(visible);
    yawLabel.setVisible(visible);
    rollValue.setVisible(visible);
    pitchValue.setVisible(visible);
    yawValue.setVisible(visible);
}

void ThreeDViewWindow::updateAxesConventionLabel() {
    axesConventionLabel.setText(toString(readFromValueTree().axesConvention));
    axesConventionLabel.setVisible(readFromValueTree().axesEnabled && compactView == false);
}

void ThreeDViewWindow::updateAhrsStatusVisibility() {
    ahrsStatusLabel.setVisible(settingsTree.getProperty("ahrsStatusEnabled", true) && compactView == false);
}

juce::PopupMenu ThreeDViewWindow::getMenu() {
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Default View", true, false, [&] {
        const auto size = settingsTree.getProperty(WindowIds::size);
        settingsTree.removeAllProperties(nullptr);
        if (size.isVoid() == false) {
            settingsTree.setProperty(WindowIds::size, size, nullptr);
        }
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("World", true, threeDView.getSettings().worldEnabled, [&] {
        auto settings = threeDView.getSettings();
        settings.worldEnabled = !settings.worldEnabled;
        writeToValueTree(settings);
    });
    menu.addItem("3D Model", true, threeDView.getSettings().modelEnabled, [&] {
        auto settings = threeDView.getSettings();
        settings.modelEnabled = !settings.modelEnabled;
        writeToValueTree(settings);
    });
    menu.addItem("Compass", true, threeDView.getSettings().compassEnabled, [&] {
        auto settings = threeDView.getSettings();
        settings.compassEnabled = !settings.compassEnabled;
        writeToValueTree(settings);
    });
    menu.addItem("Euler Angles", compactView == false, settingsTree.getProperty("eulerAnglesEnabled", true) && (compactView == false), [&] {
        settingsTree.setProperty("eulerAnglesEnabled", (bool) settingsTree.getProperty("eulerAnglesEnabled", true) == false, nullptr);
    });
    menu.addItem("AHRS Status", compactView == false, settingsTree.getProperty("ahrsStatusEnabled", true) && (compactView == false), [&] {
        settingsTree.setProperty("ahrsStatusEnabled", (bool) settingsTree.getProperty("ahrsStatusEnabled", true) == false, nullptr);
    });
    menu.addItem("Axes", compactView == false, threeDView.getSettings().axesEnabled && (compactView == false), [&] {
        auto settings = threeDView.getSettings();
        settings.axesEnabled = !settings.axesEnabled;
        writeToValueTree(settings);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("3D MODEL"), nullptr);

    juce::String autoText = "Auto";
    if (currentModel == "auto" && threeDView.getModel() != juce::File()) {
        autoText += " (" + threeDView.getModel().getFileNameWithoutExtension() + ")";
    }
    menu.addItem(autoText, true, currentModel == "auto", [&] {
        settingsTree.setProperty("model", "auto", nullptr);
    });
    for (const auto &model: Models::getAll()) {
        menu.addItem(model.getFileNameWithoutExtension(), true, currentModel == model.getFileName(), [&, model] {
            settingsTree.setProperty("model", model.getFileName(), nullptr);
        });
    }
    menu.addItem("Add 3D Model...", [&] {
        fileChooser = Models::add([&](const auto &file) {
            settingsTree.setProperty("model", file.getFileName(), nullptr);
        });
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("AXES CONVENTION"), nullptr);
    menu.addItem("North, West, Up (NWU)", true, threeDView.getSettings().axesConvention == ThreeDView::AxesConvention::nwu, [&] {
        auto settings = threeDView.getSettings();
        settings.axesConvention = ThreeDView::AxesConvention::nwu;
        writeToValueTree(settings);
    });
    menu.addItem("East, North, Up (ENU)", true, threeDView.getSettings().axesConvention == ThreeDView::AxesConvention::enu, [&] {
        auto settings = threeDView.getSettings();
        settings.axesConvention = ThreeDView::AxesConvention::enu;
        writeToValueTree(settings);
    });
    menu.addItem("North, East, Down (NED)", true, threeDView.getSettings().axesConvention == ThreeDView::AxesConvention::ned, [&] {
        auto settings = threeDView.getSettings();
        settings.axesConvention = ThreeDView::AxesConvention::ned;
        writeToValueTree(settings);
    });

    return menu;
}

void ThreeDViewWindow::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) {
    if (treeWhosePropertyHasChanged != settingsTree) {
        return;
    }

    if (property.toString() == "model") {
        updateModel();
        return;
    }

    updateEulerAnglesVisibilities();
    updateAhrsStatusVisibility();
    updateAxesConventionLabel();

    threeDView.setSettings(readFromValueTree());
}
