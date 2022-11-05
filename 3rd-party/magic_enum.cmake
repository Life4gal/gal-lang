CPMFindPackage(
		NAME magic_enum
		GIT_TAG v0.8.1
		GITHUB_REPOSITORY "Neargye/magic_enum"
		OPTIONS ""
)

CPM_link_libraries_APPEND(magic_enum PRIVATE)
