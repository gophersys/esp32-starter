# Sources for the `app` target. Paths are relative to THIS directory.
# Adding a new .c file to the app == adding one line here.
target_sources(app PRIVATE
    main.c
    checks.c
    exercises_c.c
    exercises_zephyr.c
    kv.c
)
