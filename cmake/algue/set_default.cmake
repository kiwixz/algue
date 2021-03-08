function (set_default variable value type)
    if (NOT ${variable})
        message(STATUS "defaulting ${variable} to ${value}")
        set(${variable} "${value}" CACHE ${type} "")
    endif ()
endfunction ()
