# T66 Cursor Rules (Non-Negotiable)

## Source of Truth
- C++ is the source of truth for gameplay logic and systems.
- Blueprints are used only for visuals/presentation (UI layout, animations, VFX hooks) unless explicitly required.

## Architecture Boundaries
- UI does not directly talk to gameplay systems.
- UI ↔ Gameplay communication goes through the Bridge layer only.

## Production Rules
- No temporary systems, no throwaway code, no “we’ll redo it later.”
- Build the final intended structure from the start.
- Use descriptive, specific names (no vague names like Manager1, Temp, Test, etc.).

## Workflow
- Keep changes small and commit frequently.
- If unsure about requirements, stop and ask for clarification before implementing.
