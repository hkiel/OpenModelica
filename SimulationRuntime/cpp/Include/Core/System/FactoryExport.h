#pragma once

#if defined(__vxworks) || defined(__TRICORE__) || defined(RUNTIME_STATIC_LINKING) 

#define BOOST_EXTENSION_SYSTEM_DECL
#define BOOST_EXTENSION_EVENTHANDLING_DECL
#define BOOST_EXTENSION_ALGLOOPDEFAULTIMPL_DECL
#define BOOST_EXTENSION_SIMVARS_DECL
#elif defined(OMC_BUILD) || defined(SIMSTER_BUILD)
#define BOOST_EXTENSION_SYSTEM_DECL BOOST_EXTENSION_EXPORT_DECL
#define BOOST_EXTENSION_EVENTHANDLING_DECL BOOST_EXTENSION_EXPORT_DECL
#define BOOST_EXTENSION_ALGLOOPDEFAULTIMPL_DECL BOOST_EXTENSION_EXPORT_DECL
#define BOOST_EXTENSION_SIMVARS_DECL BOOST_EXTENSION_EXPORT_DECL
#else
    error "operating system not supported"
#endif



