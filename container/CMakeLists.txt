#========================================================================= 
set(myarray
    myarray.h
)

add_library(myarray ${myarray})
set_source_files_properties(myarray.h PROPERTIES LANGUAGE CXX)

#========================================================================= 
set(myoptional
    myoptional.h
)

add_library(myoptional ${myoptional})
set_source_files_properties(myoptional.h PROPERTIES LANGUAGE CXX)