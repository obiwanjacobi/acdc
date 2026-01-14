# Simple LCD UI Framework

Control Types:

- Control. A basic control without user interaction.
- Input. The control receives input from user interaction and can be focussed and/or selected.
- Panel. A container for multiple controls. Usually performs layout and keeps track of current selected control.
- Page. A complete UI page with multiple lines.

Control States:

- Disabled. The control does not receive input but is displayed as read-only.  
- Hidden. The control does not receive input and is not displayed.
- Focussed. The control has the focus and is displayed high-lighted with a cursor. Only one control has focus at a time.
- Selected. The control is displayed with an edit cursor. Only one control can be selected at a time.

Navigation Commands:

- Left. Horizontal navigation between `InputControl`s or between characters in an `EditControl`.
- Right. Horizontal navigation between `InputControl`s or between characters in an `EditControl`.
- Up. Vertical navigation between `InputControl`s/`Line`s or value selection in an `EditControl`.
- Down. Vertical navigation between `InputControl`s/`Line`s or value selection in an `EditControl`.
- Enter. Enter control-Focus then control-Selected or toggle the `ButtonControl` state.
- Exit. Exits control-Selected then control-Focus (then `Page`).

Control Class Hierarchy:

- `Control`
  - `LabelControl`
  - `InputControl`
    - `UpDownControl`
      - `EditControl`
        - `TextControl`
      - `SelectControl`
        - `OptionsControl`
    - `ButtonControl`
    - `Panel`
      - `PanelControlContainer` (+`ControlContainer`)
        - `HorizontalPanel`
          - `Line`
        - `VerticalPanel`
          - `Page`
        - `PageController`

A typical LCD Page layout (2 lines):

- `Page: VerticalPanel: PanelControlContainer: Panel: InputControl: Control`
  - `Line: HorizonatalPanel: PanelControlContainer: Panel: InputControl: Control`
    - `LabelControl: Control`
    - `TextControl: EditControl: UpDownControl: InputControl: Control`
  - `Line: HorizonatalPanel: PanelControlContainer: Panel: InputControl: Control`
    - `LabelControl: Control`
    - `TextControl: EditControl: UpDownControl: InputControl: Control`

## Framework Classes

### `Control` (class)

A base class for all controls.

- Position. Position of the control inside its container.
- State. Disabled/Focussed/Selected. Manages state changes.
- ControlCast/IsType. Lightweight runtime type information (`ControlTypes`).
- Display. Called to draw the control.

### `LabelControl` extends `Control` (class)

A read-only display text for labelling other controls.

### `InputControl` extends `Control` (class)

A base class for all controls that use the `NavigationCommands` to allow editing their content.

This class allows registering a `InputControlHandler` that can intercept navigation commands and receives InputControl events.

#### `InputControl` Events

The actual values of each event has to be unique across all Controls.
The value 0x00 is not used as an event identifier.

- `InputControl` ChangeState (0x01). When an `InputControl` is set to a new `ControlState`.
- `ButtonControl` ButtonChangeState (0x02). When the `ButtonControl` state is toggled (on/off).
- `Panel` PanelChangeState (0x03). When the `Panel`'s current control is changed (2x).

### `UpDownControl` extends `InputControl` (template)

Manages changing a value with the Up/Down navigation Commands in edit mode (Selected).

### `EditControl` extends `UpDownControl` (class)

Implements text editing in a control using a character provider (iterator).

### `TextControl` extends `EditControl` (template)

Manages displaying and editing a fixes-size string.

### `SelectControl` extends `UpDownControl` (template)

Implements a control to select from a list of options using an options provider (iterator).

### `OptionsControl` extends `SelectControl` (template)

Implements options selection based on an array with pointers to text.

### `ButtonControl` extends `InputControl` (template)

Manages displaying a toggle button (on/off).

### `Panel` extends `InputControl` (class)

Keeps track of the current active `InputControl`.

### `ControlContainer` (template)

Fixed-size collection of `Control` instance pointers.

Allows adding and iteration (next/previous) of control pointers.

### `PanelControlContainer` extends `Panel` and `ControlContainer` (template)

A fixed-size container base class for managing multiple `InputControl`s.

### `HorizontalPanel` extends `PanelControlContainer` (template)

A fixed size control container that lays out the controls horizontally.

### `Line` extends `HorizontalPanel` (template)

A fixed-size control container for laying out a single display line.

### `VerticalPanel` extends `PanelControlContainer` (template)

A fixed size control container that lays out the controls vertically.

### `Page` extends `VerticalPanel` (template)

A fixed-sized `Line` containter for laying out an entire display page.

### `PageController` extends `PanelControlContainer` (template)

A fixed-size container of pages that manages page transitions.

### `DisplayWriter` (class)

An abstraction to decouple drawing controls and UI in general onto any display.

- `Display`. Draw text.
- `GoTo`. Move current drawing position.
- `EnableCursor`. Display cursor (two types)

---

## TODO

- HorizontalScrollPanel (uses HD44780_ViewPort - `DisplayWriter` extension?) handles more characters than display width.
- MenuPage (VerticalScrollPanel)
- Decide if Page uses PanelControlContainer, HorizontalPanel or Line as 'line'.
- Page: have method to know if any control is focused and/or selected.
- Test runtime changes of ControlState::Hidden etc.
- Because we are working with /0 terminated strings, displaying custom character '0' does not work.
