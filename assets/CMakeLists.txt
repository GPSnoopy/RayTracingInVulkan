
file(GLOB font_files fonts/*.ttf)
file(GLOB model_files models/*.obj models/*.mtl)
file(GLOB shader_files shaders/*.vert shaders/*.frag shaders/*.rgen shaders/*.rchit shaders/*.rint shaders/*.rmiss)
file(GLOB texture_files textures/*.jpg textures/*.png textures/*.txt)

file(GLOB shader_extra_files shaders/*.glsl)
set_source_files_properties(${shader_extra_files} PROPERTIES HEADER_FILE_ONLY TRUE)

# Shader compilation
foreach(shader ${shader_files})
	#message("SHADER: ${shader}")
	get_filename_component(file_name ${shader} NAME)
	get_filename_component(full_path ${shader} ABSOLUTE)
	set(output_dir ${CMAKE_CURRENT_BINARY_DIR}/shaders)
	set(output_file ${output_dir}/${file_name}.spv)
	set(compiled_shaders ${compiled_shaders} ${output_file})
	set(compiled_shaders ${compiled_shaders} PARENT_SCOPE)
	set_source_files_properties(${shader} PROPERTIES HEADER_FILE_ONLY TRUE)
        add_custom_command(
            OUTPUT ${output_file}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}
            COMMAND ${Vulkan_GLSLANG_VALIDATOR} --target-env vulkan1.2 -V ${full_path} -o ${output_file}
            DEPENDS ${full_path}
        )
endforeach()

macro(copy_assets asset_files dir_name copied_files)
	foreach(asset ${${asset_files}})
		#message("asset: ${asset}")
		get_filename_component(file_name ${asset} NAME)
		get_filename_component(full_path ${asset} ABSOLUTE)
		set(output_dir ${CMAKE_CURRENT_BINARY_DIR}/${dir_name})
		set(output_file ${output_dir}/${file_name})
		set(${copied_files} ${${copied_files}} ${output_file})
		set(${copied_files} ${${copied_files}} PARENT_SCOPE)
		set_source_files_properties(${asset} PROPERTIES HEADER_FILE_ONLY TRUE)
		if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
			add_custom_command(
				OUTPUT ${output_file}
				#COMMAND mklink \"${output_file}\" \"${full_path}\"
				COMMAND xcopy \"${full_path}\" \"${output_file}*\" /Y /Q /F
				DEPENDS ${full_path}
			)
		else()
			add_custom_command(
				OUTPUT ${output_file}
				COMMAND mkdir --parents ${output_dir} && cp --force --link \"${full_path}\" \"${output_file}\"
				DEPENDS ${full_path}
			)
		endif()
	endforeach()
endmacro()

copy_assets(font_files fonts copied_fonts)
copy_assets(model_files models copied_models)
copy_assets(texture_files textures copied_textures)
	
source_group("Fonts" FILES ${font_files})
source_group("Models" FILES ${model_files})
source_group("Shaders" FILES ${shader_files} ${shader_extra_files})
source_group("Textures" FILES ${texture_files})

add_custom_target(
	Assets 
	DEPENDS ${copied_fonts} ${copied_models} ${compiled_shaders} ${copied_textures} 
	SOURCES ${font_files} ${model_files} ${shader_files} ${shader_extra_files} ${texture_files})
