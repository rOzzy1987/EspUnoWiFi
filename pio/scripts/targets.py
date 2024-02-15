Import("env")

env.AddCustomTarget(
    name="Prettify",
    dependencies=None,
    actions=[
        "npm run prettify"
    ],
    title="Prettifier",
    description="Prettify resource files",
    always_build=False
)