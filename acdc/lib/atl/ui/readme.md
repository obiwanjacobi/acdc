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
- ControlCast/IsType. Lightweight runtime type information.
- Display. Called to draw the control.

### `LabelControl` extends `Control` (class)

A read-only display text for labelling other controls.

### `InputControl` extends `Control` (class)

A base class for all controls that use the `NavigationCommands` to allow editing their content.

### `UpDownControl` extends `InputControl` (template)

Manages changing a value with the Up/Down navigation Commands in edit mode (Selected).

### `EditControl` extends `UpDownControl` (class)

Implements text editing in a control using a character provider (iterator).

### `TextControl` extends `EditControl` (template)

Manages displaying and editing a fixes-size string.

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

- VerticalScrollPanel (code manages lines to display)
- HorizontalScrollPanel (uses HD44780_ViewPort - `DisplayWriter` extension?)
- MenuPage (VerticalScrollPanel)
