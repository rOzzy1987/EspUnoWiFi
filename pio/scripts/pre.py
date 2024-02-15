Import("env")

def tsc(source, target, env):
    print("Running TSC")
    env.Execute("tsc")
    print("TSC done")

def compile(source, target, env):
    print("Running resource compilation")
    env.Execute("node compile-res "+ env)
    print("Resource compilation done")



env.AddPreAction("tsc", tsc)
env.AddPreAction("comp", compile)

