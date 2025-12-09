# 1. Сначала utils/sc-builder — это основная библиотека
add_subdirectory(utils/sc-builder-local)

# 2. Потом utils (base64 etc)
add_subdirectory(utils)

# 3. Потом общие компоненты
add_subdirectory(common)

# 4. Потом все модули, которые зависят от sc-builder-local
add_subdirectory(er-diagram-module)
add_subdirectory(state-diagram-module)
add_subdirectory(use-case-diagram-module)
