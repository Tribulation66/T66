# SetupHeroSelection.py - Creates the Hero Selection screen
import unreal

SCREEN_PATH = "/Game/Tribulation66/Content/UI/Screens/WBP_Screen_HeroSelect_Solo"

# Hero data (matches C++ T66SelectionSubsystem)
HEROES = [
    {"name": "Warrior", "color": (0.8, 0.2, 0.2), "desc": "A fierce melee combatant with high defense."},
    {"name": "Mage", "color": (0.2, 0.2, 0.9), "desc": "A powerful spellcaster with devastating magic."},
    {"name": "Rogue", "color": (0.4, 0.1, 0.6), "desc": "A swift assassin who strikes from the shadows."},
    {"name": "Ranger", "color": (0.2, 0.7, 0.2), "desc": "A skilled archer with deadly precision."},
    {"name": "Paladin", "color": (0.9, 0.85, 0.2), "desc": "A holy knight with healing abilities."},
    {"name": "Berserker", "color": (0.9, 0.5, 0.1), "desc": "A raging warrior who grows stronger in combat."},
]

def setup_hero_selection():
    print("[T66] Setting up Hero Selection screen...")
    
    # Load or create the widget
    bp = unreal.load_asset(SCREEN_PATH)
    if not bp:
        print(f"[T66] ERROR: Could not load {SCREEN_PATH}")
        return False
    
    # Get the widget tree
    widget_tree = bp.get_editor_property("widget_tree")
    if not widget_tree:
        print("[T66] ERROR: No widget tree")
        return False
    
    # Clear existing widgets
    root = widget_tree.get_editor_property("root_widget")
    
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
    
    # === TOP: TITLE + HERO BELT ===
    title = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_Title"))
    title.set_text(unreal.Text("HERO SELECTION"))
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
    
    # Hero belt (horizontal box of hero buttons)
    belt_hb = widget_tree.construct_widget(unreal.HorizontalBox, unreal.Name("HB_HeroBelt"))
    main_vb.add_child_to_vertical_box(belt_hb)
    belt_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(belt_hb)
    if belt_slot:
        belt_slot.set_horizontal_alignment(unreal.HorizontalAlignment.CENTER)
        belt_slot.set_padding(unreal.Margin(0, 10, 0, 20))
    
    # Create hero buttons in belt
    for i, hero in enumerate(HEROES):
        hero_btn = widget_tree.construct_widget(unreal.Button, unreal.Name(f"Btn_Hero_{i}"))
        r, g, b = hero["color"]
        hero_btn.set_editor_property("background_color", unreal.LinearColor(r, g, b, 1.0))
        belt_hb.add_child_to_horizontal_box(hero_btn)
        
        hero_slot = unreal.WidgetLayoutLibrary.slot_as_horizontal_box_slot(hero_btn)
        if hero_slot:
            hero_slot.set_padding(unreal.Margin(5, 0, 5, 0))
        
        # Size box to constrain button size
        # Note: Button already added, we'll set size via style
        
        # Add text inside button
        hero_text = widget_tree.construct_widget(unreal.TextBlock, unreal.Name(f"Text_Hero_{i}"))
        hero_text.set_text(unreal.Text(hero["name"][:1]))  # First letter
        hero_text.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(1, 1, 1, 1)))
        font = hero_text.get_editor_property("font")
        font.size = 24
        hero_text.set_editor_property("font", font)
        hero_btn.add_child(hero_text)
    
    # === CENTER: HERO INFO PANEL ===
    info_border = widget_tree.construct_widget(unreal.Border, unreal.Name("Border_HeroInfo"))
    info_border.set_editor_property("brush_color", unreal.LinearColor(0.1, 0.1, 0.15, 0.9))
    info_border.set_editor_property("padding", unreal.Margin(20, 20, 20, 20))
    main_vb.add_child_to_vertical_box(info_border)
    info_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(info_border)
    if info_slot:
        info_slot.set_size(unreal.SlateChildSize(1.0, unreal.SlateSizeRule.FILL))
    
    # Info content
    info_vb = widget_tree.construct_widget(unreal.VerticalBox, unreal.Name("VB_HeroInfo"))
    info_border.add_child(info_vb)
    
    # Hero name
    hero_name = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_HeroName"))
    hero_name.set_text(unreal.Text("WARRIOR"))
    hero_name.set_editor_property("justification", unreal.TextJustify.CENTER)
    font = hero_name.get_editor_property("font")
    font.size = 48
    hero_name.set_editor_property("font", font)
    hero_name.set_editor_property("color_and_opacity", unreal.SlateColor(unreal.LinearColor(0.8, 0.2, 0.2, 1)))
    info_vb.add_child_to_vertical_box(hero_name)
    
    # Hero description
    hero_desc = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_HeroDesc"))
    hero_desc.set_text(unreal.Text("A fierce melee combatant with high defense."))
    hero_desc.set_editor_property("justification", unreal.TextJustify.CENTER)
    font = hero_desc.get_editor_property("font")
    font.size = 18
    hero_desc.set_editor_property("font", font)
    info_vb.add_child_to_vertical_box(hero_desc)
    desc_slot = unreal.WidgetLayoutLibrary.slot_as_vertical_box_slot(hero_desc)
    if desc_slot:
        desc_slot.set_padding(unreal.Margin(0, 20, 0, 40))
    
    # Preview placeholder
    preview_text = widget_tree.construct_widget(unreal.TextBlock, unreal.Name("Text_Preview"))
    preview_text.set_text(unreal.Text("[ 3D Hero Preview ]"))
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
    create_action_button("Btn_ChooseCompanion", "Choose Companion")
    create_action_button("Btn_EnterTribulation", "ENTER THE TRIBULATION")
    
    # Save
    bp.modify()
    unreal.EditorAssetLibrary.save_asset(SCREEN_PATH)
    
    print("[T66] Hero Selection screen setup complete!")
    return True

if __name__ == "__main__":
    setup_hero_selection()
