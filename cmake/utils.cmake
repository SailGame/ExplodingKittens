function(add_pb_files proto_files)
    foreach(proto_file ${proto_files})
        # get dir name, such as core, uno, texas ...
        get_filename_component(dir ${proto_file} DIRECTORY)
        get_filename_component(dir ${dir} NAME)

        # get suffix, such as provider, error, types ...
        get_filename_component(suffix ${proto_file} NAME_WE)

        set(pb_hdrs ${pb_hdrs}
            ${pb_dir}/${dir}/${suffix}.pb.h
            ${pb_dir}/${dir}/${suffix}.grpc.pb.h)
        set(pb_srcs ${pb_srcs} 
            ${pb_dir}/${dir}/${suffix}.pb.cc
            ${pb_dir}/${dir}/${suffix}.grpc.pb.cc)
    endforeach()

    set(pb_hdrs ${pb_hdrs} PARENT_SCOPE)
    set(pb_srcs ${pb_srcs} PARENT_SCOPE)
endfunction()
