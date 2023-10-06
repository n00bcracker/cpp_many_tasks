add_catch(test_raytracer_b2 tests/test.cpp)

if (TEST_SOLUTION)
    target_include_directories(test_raytracer_b2 PRIVATE ../tests/raytracer-geom)
    target_include_directories(test_raytracer_b2 PRIVATE ../tests/raytracer-reader)
    target_include_directories(test_raytracer_b2 PRIVATE ../tests/raytracer)
else()
    target_include_directories(test_raytracer_b2 PRIVATE ../raytracer-geom)
    target_include_directories(test_raytracer_b2 PRIVATE ../raytracer-reader)
endif()
target_include_directories(test_raytracer_b2 PRIVATE ../raytracer)

target_link_libraries(test_raytracer_b2 PRIVATE ${PNG_LIBRARY})
target_include_directories(test_raytracer_b2 PRIVATE ${PNG_INCLUDE_DIRS})
