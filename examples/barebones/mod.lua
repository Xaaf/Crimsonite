-- 
-- mod.lua holds the metadata for a mod. This is where we specify the mod's name,
-- description, version, author, and any dependencies on other mods!
-- 
return {
    -- The name of the mod, which will be displayed in the mod list in-game.
    name = "Example Mod",
    -- A unique identifier for the mod, used for things like dependencies. Should be in lower_snake_case.
    mod_id = "example_mod", 
    -- A brief description of the mod. This can be anything you want, and will also be shown in the mod list.
    description = "An example mod for the Crimsonite framework!",
    -- The version of the mod, which is represented with just a string so it can be whatever versioning
    -- system you prefer.
    version = "1.0.0-example",
    -- The authors of the mod. This can be a single string or a list of strings if there are multiple authors.
    author = "Xaaf",
    -- authors = { "Xaaf", "faaX" },
    dependencies = {
        -- Here we could specify other mods that this mod depends on, e.g.:
        -- { name = "SomeOtherMod", version = ">=1.0.0" } for minimum versions
        -- { name = "SomeOtherMod", version = "= 1.0.0" } for exact versions
    }
}