# CreateSimpleLevel.py - Clean simple level
import unreal

LEVEL_PATH = "/Game/Tribulation66/Content/Maps/Lvl_Simple"
CUBE_MESH_PATH = "/Engine/BasicShapes/Cube.Cube"

def create_simple_level():
    print("[T66] Creating clean simple level...")
    
    # Create brand new empty level (this clears everything)
    print("[T66] Creating new empty level...")
    unreal.EditorLevelLibrary.new_level(LEVEL_PATH)
    
    # Load cube mesh
    cube_mesh = unreal.load_asset(CUBE_MESH_PATH)
    if not cube_mesh:
        print("[T66] ERROR: Could not load cube mesh!")
        return False
    
    # === FLOOR ===
    print("[T66] Adding floor...")
    floor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor, 
        unreal.Vector(0, 0, 0),
        unreal.Rotator(0, 0, 0)
    )
    if floor:
        floor.set_actor_label("Floor")
        floor.static_mesh_component.set_static_mesh(cube_mesh)
        floor.set_actor_scale3d(unreal.Vector(100, 100, 1))
        floor.static_mesh_component.set_mobility(unreal.ComponentMobility.MOVABLE)
        print("[T66] Floor created")
    
    # === SINGLE DIRECTIONAL LIGHT with high priority ===
    print("[T66] Adding directional light...")
    sun = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.DirectionalLight,
        unreal.Vector(0, 0, 500),
        unreal.Rotator(-50, 30, 0)
    )
    if sun:
        sun.set_actor_label("MainSun")
        light = sun.light_component
        light.set_mobility(unreal.ComponentMobility.MOVABLE)
        light.set_intensity(10.0)
        # Set forward shading priority to avoid conflicts
        light.set_editor_property("forward_shading_priority", 1)
        print("[T66] Directional light created with forward_shading_priority=1")
    
    # === SKY LIGHT for ambient ===
    print("[T66] Adding sky light...")
    sky = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.SkyLight,
        unreal.Vector(0, 0, 1000),
        unreal.Rotator(0, 0, 0)
    )
    if sky:
        sky.set_actor_label("SkyLight")
        sky.light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
        sky.light_component.set_intensity(1.0)
        sky.light_component.set_editor_property("real_time_capture", True)
        print("[T66] Sky light created")
    
    # === PLAYER START ===
    print("[T66] Adding player start...")
    player_start = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart,
        unreal.Vector(0, 0, 150),
        unreal.Rotator(0, 0, 0)
    )
    if player_start:
        player_start.set_actor_label("PlayerStart")
        print("[T66] Player start created at Z=150")
    
    # === SAVE ===
    print("[T66] Saving level...")
    unreal.EditorLevelLibrary.save_current_level()
    
    print("[T66] ========================================")
    print("[T66] SUCCESS! Clean level created")
    print("[T66] - 1 Floor")
    print("[T66] - 1 Directional Light (priority=1)")
    print("[T66] - 1 Sky Light")
    print("[T66] - 1 Player Start")
    print("[T66] ========================================")
    return True

create_simple_level()
