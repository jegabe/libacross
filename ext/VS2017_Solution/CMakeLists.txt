cmake_minimum_required(VERSION 3.14)
project(lia)

set(Sources
	Dummy.cpp
	../../src/lia/detail/PushWarnings.h
	../../src/lia/detail/PopWarnings.h
	../../src/lia/detail/VectorApiMixin.h
	../../src/lia/detail/VectorProxy.h
	../../src/lia/defs.h
	../../src/lia/IVector.h
	../../src/lia/VectorRef.h
)

add_executable(lia
	${Sources}
)

target_include_directories(lia PRIVATE ../../src)
target_include_directories(lia PRIVATE .)

source_group(TREE ${PROJECT_SOURCE_DIR}/../.. FILES ${Sources})
