// natNameFinder.cc - native helper methods for NameFiner.java

/* Copyright (C) 2002  Free Software Foundation, Inc

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */

/**
 * @author Mark Wielaard (mark@klomp.org)
 * Based on the old name-finder.cc by Andrew Haley <aph@cygnus.com>.
 */

#include <config.h>

#include <gcj/cni.h>
#include <jvm.h>
#include <java/lang/String.h>
#include <java/lang/StackTraceElement.h>

#include <gnu/gcj/runtime/NameFinder.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

java::lang::String*
gnu::gcj::runtime::NameFinder::getExecutable (void)
{
    return JvNewStringLatin1 (_Jv_ThisExecutable ());
}

java::lang::String*
gnu::gcj::runtime::NameFinder::getAddrAsString(RawData* addrs, jint n)
{
  void **p = (void **) addrs;
  typedef unsigned word_t __attribute ((mode (word)));
  word_t w = (word_t) p[n];
  int digits = sizeof (void *) * 2;
  char hex[digits+5];

  strcpy (hex, "0x");
  for (int i = digits - 1; i >= 0; i--)
    {
      int digit = w % 16;

      w /= 16;
      hex[i+2] = digit > 9 ? 'a' + digit - 10 : '0' + digit;
    }
  hex [digits+2] = 0;

  return JvNewStringLatin1(hex);
}

java::lang::StackTraceElement*
gnu::gcj::runtime::NameFinder::dladdrLookup(RawData* addrs, jint n)
{
#if defined (HAVE_DLFCN_H) && defined (HAVE_DLADDR)
  extern char **_Jv_argv;
  char name[1024];
  char file_name[1024];
  void **stack = (void **) addrs;
  void* p = stack[n];
  Dl_info dl_info;
   
  if (dladdr (p, &dl_info))
    {
      if (dl_info.dli_fname)
        strncpy (file_name, dl_info.dli_fname, sizeof file_name);
      if (dl_info.dli_sname)
        strncpy (name, dl_info.dli_sname, sizeof name);
     
     /* Don't trust dladdr() if the address is from the main program. */
     if (dl_info.dli_fname != NULL
         && dl_info.dli_sname != NULL
         && (_Jv_argv == NULL || strcmp (file_name, _Jv_argv[0]) != 0))
       return createStackTraceElement (JvNewStringLatin1 (name),
				       JvNewStringLatin1 (file_name));
    }
#endif
  return NULL;
}
