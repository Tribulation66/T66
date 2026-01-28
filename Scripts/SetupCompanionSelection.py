# SetupCompanionSelection.py - Creates the Companion Selection screen
import unreal

SCREEN_PATH = "/Game/Tribulation66/Content/UI/Screens/WBP_Screen_CompanionSelect"

# Companion data (matches C++ T66SelectionSubsystem)
COMPANIONS = [
    {"name": "Lumina", "color": (0.9, 0.9, 0.5), "desc": "She tells me... of a time when the light was all there was."},
    {"name": "Umbra", "color": (0.3, 0.3, 0.4), "desc": "She tells me... secrets whispered in the darkness."},
    {"name": "Ember", "color": (1.0, 0.4, 0.1), "desc": "She tells me... of fires that never die."},
    {"name": "Crystal", "color": (0.5, 0.8, 1.0), "desc": "She tells me... of frozen dreams beneath the ice."},
]

def setup_companion_selection():
    print("[T66] Setting up Companion Selection screen...")
    
    # Load or create the widget
    bp = unreal.load_asset(SCREEN_PATH)
    if not bp:
        # Need to create it first
        print(f"[T66] Creating new widget blueprint at {SCREEN_PATH}")
        factory = unreal.WidgetBlueprintFactory()
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        bp = asset_tools.create_asset("WBP_Screen_CompanionSelect", 
            "/Game/Tribulation66/Content/UI/Screens", 
            unreal.WidgetBlueprint, 
            factory)
        if not bp:
            print("[T66] ERROR: Could not create widget blueprint")
            return False
    
    # Get the widget tree
    widget_tree = bp.get_editor_property("widget_tree")
    if not widget_tree:
        print("[T66] ERROR: No widget tree")
        return False
    
    # Create new canvas as root
    canvas = widget_tree.construct_widget(unreal.CanvasPanel, unreal.Name("Root_Canvas"))
    widget_tree.set_editor_property("root_widget", canvas)
    
    # === OPAQUE BACKGROUND ===
    bg = widget_tree.construct_widget(unreal.Border, unreal.Name("Background"))
    bg.set_editor_property("brush_color", unreal.LinearColor(0.05, 0.05, 0.08, 1.0))
    canvas.add_child_to_canvas(bg)
    bg_slot = unreal.WidgetLayoutLibrary.slot_as_canvas_slot(bg)
    if bg_slot:
        bg_slot.set_anchors(unreal.Anchors(0, 0, 1, 1))
        bg_slot.set_offsets(unreal.Margin(0, 0, 0, 0))
    
    # === MAIN VERTICAL LAYOUT ===
    main_vb = widget_tree.construct_widget(unreal.VerticalBox, unreal.Name("VB_Main"))
    canvas.add_child_to_canvas(main_vb)
    main_slot = unreal.WidgetLayoutLibrary.slot_as_canvas_slot(main_vb)
    if main_slot:
        main_slot.set_anchors(unreal.Anchors(0, 0, 1, 1))
        main_slot.set_offsets(unreal.Margin(20, 20, 20, 20))
    
    # === TOP: TITLE + COMPANION BELT ===
    title = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_Title"))
    title.set_text(unreal.Text("COMPANION SELECTION"))
    title.set_editor_property("justification", unreal.TextJustify.CENTER)
    font = title.get_editor_property("font")
    font.size = 36
    title.set_editor_property("font", font)
    title.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(1, 1, 1, 1)))
    main_vb.add_child_to_vertical_box(title)
    title_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(title)
    if title_slot:
        title_slot.set_horizontal_alignment(unreal.HorizontalAlignment.CENTER)
        title_slot.set_padding(unreal.Margin(0, 0, 0, 10))
    
    # Companion belt (horizontal box of companion buttons)
    belt_hb = widget_tree.construct_widget(unreal.HorizontalBox, unreal.Name("HB_CompanionBelt"))
    main_vb.add_child_to_vertical_box(belt_hb)
    belt_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(belt_hb)
    if belt_slot:
        belt_slot.set_horizontal_alignment(unreal.HorizontalAlignment.CENTER)
        belt_slot.set_padding(unreal.Margin(0, 10, 0, 20))
    
    # "No Companion" button first
    no_comp_btn = widget_tree.construct_widget(unreal.Button, unreal.Name("Btn_NoCompanion"))
    no_comp_btn.set_editor_property("background_color", unreal.LinearColor(0.3, 0.3, 0.3, 1.0))
    belt_hb.add_child_to_horizontal_box(no_comp_btn)
    no_slot = unreal.WidgetLayoutLibrary.slot_as_horizontal_box_slot(no_comp_btn)
    if no_slot:
        no_slot.set_padding(unreal.Margin(5, 0, 5, 0))
    no_text = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_NoCompanion"))
    no_text.set_text(unreal.Text("X"))
    no_text.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(1, 0.3, 0.3, 1)))
    font = no_text.get_editor_property("font")
    font.size = 24
    no_text.set_editor_property("font", font)
    no_comp_btn.add_child(no_text)
    
    # Create companion buttons in belt
    for i, comp in enumerate(COMPANIONS):
        comp_btn = widget_tree.construct_widget(unreal.Button, unreal.Name(f"Btn_Companion_{i}"))
        r, g, b = comp["color"]
        comp_btn.set_editor_property("background_color", unreal.LinearColor(r, g, b, 1.0))
        belt_hb.add_child_to_horizontal_box(comp_btn)
        
        comp_slot = unreal.WidgetLayoutLibrary.slot_as_horizontal_box_slot(comp_btn)
        if comp_slot:
            comp_slot.set_padding(unreal.Margin(5, 0, 5, 0))
        
        # Add text inside button
        comp_text = widget_tree.construct_widget(unreal.TextBlock, unreal.Name(f"Text_Companion_{i}"))
        comp_text.set_text(unreal.Text(comp["name"][:1]))  # First letter
        comp_text.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(1, 1, 1, 1)))
        font = comp_text.get_editor_property("font")
        font.size = 24
        comp_text.set_editor_property("font", font)
        comp_btn.add_child(comp_text)
    
    # === CENTER: COMPANION INFO PANEL ===
    info_border = widget_tree.construct_widget(unreal.Border, unreal.Name("Border_CompanionInfo"))
    info_border.set_editor_property("brush_color", unreal.LinearColor(0.1, 0.1, 0.15, 0.9))
    info_border.set_editor_property("padding", unreal.Margin(20, 20, 20, 20))
    main_vb.add_child_to_vertical_box(info_border)
    info_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(info_border)
    if info_slot:
        info_slot.set_size(unreal.SlateChildSize(1.0, unreal.SlateSizeRule.FILL))
    
    # Info content
    info_vb = widget_tree.construct_widget(unreal.VerticalBox, unreal.Name("VB_CompanionInfo"))
    info_border.add_child(info_vb)
    
    # Companion name
    comp_name = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_CompanionName"))
    comp_name.set_text(unreal.Text("LUMINA"))
    comp_name.set_editor_property("justification", unreal.TextJustify.CENTER)
    font = comp_name.get_editor_property("font")
    font.size = 48
    comp_name.set_editor_property("font", font)
    comp_name.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(0.9, 0.9, 0.5, 1)))
    info_vb.add_child_to_vertical_box(comp_name)
    
    # Companion lore
    comp_desc = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_CompanionDesc"))
    comp_desc.set_text(unreal.Text("She tells me... of a time when the light was all there was."))
    comp_desc.set_editor_property("justification", unreal.TextJustify.CENTER)
    font = comp_desc.get_editor_property("font")
    font.size = 18
    comp_desc.set_editor_property("font", font)
    info_vb.add_child_to_vertical_box(comp_desc)
    desc_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(comp_desc)
    if desc_slot:
        desc_slot.set_padding(unreal.Margin(0, 20, 0, 20))
    
    # Passive effect
    passive_text = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_Passive"))
    passive_text.set_text(unreal.Text("Passive: Heals the hero over time"))
    passive_text.set_editor_property("justification", unreal.TextJustify.CENTER)
    font = passive_text.get_editor_property("font")
    font.size = 16
    passive_text.set_editor_property("font", font)
    passive_text.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(0.5, 1, 0.5, 1)))
    info_vb.add_child_to_vertical_box(passive_text)
    
    # Preview placeholder
    preview_text = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_Preview"))
    preview_text.set_text(unreal.Text("[ 3D Companion Preview ]"))
    preview_text.set_editor_property("justification", unreal.TextJustify.CENTER)
    font = preview_text.get_editor_property("font")
    font.size = 24
    preview_text.set_editor_property("font", font)
    preview_text.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(0.5, 0.5, 0.5, 1)))
    info_vb.add_child_to_vertical_box(preview_text)
    preview_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(preview_text)
    if preview_slot:
        preview_slot.set_size(unreal.SlateChildSize(1.0, unreal.SlateSizeRule.FILL))
        preview_slot.set_horizontal_alignment(unreal.HorizontalAlignment.CENTER)
        preview_slot.set_vertical_alignment(unreal.VerticalAlignment.CENTER)
    
    # === BOTTOM: ACTION BUTTONS ===
    btn_hb = widget_tree.construct_widget(unreal.HorizontalBox, unreal.Name("HB_Buttons"))
    main_vb.add_child_to_vertical_box(btn_hb)
    btn_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(btn_hb)
    if btn_slot:
        btn_slot.set_horizontal_alignment(unreal.HorizontalAlignment.CENTER)
        btn_slot.set_padding(unreal.Margin(0, 20, 0, 0))
    
    def create_action_button(name, label, width=180):
        btn = widget_tree.construct_widget(unreal.Button, unreal.Name(name))
        btn.set_editor_property("background_color", unreal.LinearColor(0.2, 0.2, 0.3, 1.0))
        btn_hb.add_child_to_horizontal_box(btn)
        slot = unreal.WidgetLayoutLibrary.slot_as_horizontal_box_slot(btn)
        if slot:
            slot.set_padding(unreal.Margin(10, 0, 10, 0))
        
        txt = widget_tree.construct_widget(unreal.TextBlock, unreal.Name(f"Text_{name}"))
        txt.set_text(unreal.Text(label))
        font = txt.get_editor_property("font")
        font.size = 16
        txt.set_editor_property("font", font)
        txt.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(1, 1, 1, 1)))
        btn.add_child(txt)
        return btn
    
    create_action_button("Btn_Back", "Back")
    create_action_button("Btn_ConfirmCompanion", "CONFIRM COMPANION")
    
    # Save
    bp.modify()
    unreal.EditorAssetLibrary.save_asset(SCREEN_PATH)
    
    print("[T66] Companion Selection screen setup complete!")
    return True

if __name__ == "__main__":
    setup_companion_selection()
