/*************************************************
* Random Number Generator Base Source File       *
* (C) 1999-2008 Jack Lloyd                       *
*************************************************/

#include <botan/rng.h>
#include <botan/util.h>
#include <botan/parsing.h>
#include <botan/timers.h>

#if defined(BOTAN_HAS_RANDPOOL)
  #include <botan/lookup.h>
  #include <botan/randpool.h>

#if defined(BOTAN_HAS_X931_RNG)
  #include <botan/x931_rng.h>
#endif

#endif

#if defined(BOTAN_HAS_TIMER_HARDWARE)
  #include <botan/tm_hard.h>
#elif defined(BOTAN_HAS_TIMER_POSIX)
  #include <botan/tm_posix.h>
#elif defined(BOTAN_HAS_TIMER_UNIX)
  #include <botan/tm_unix.h>
#elif defined(BOTAN_HAS_TIMER_WIN32)
  #include <botan/tm_win32.h>
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_DEVICE)
  #include <botan/es_dev.h>
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_EGD)
  #include <botan/es_egd.h>
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_UNIX)
  #include <botan/es_unix.h>
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_BEOS)
  #include <botan/es_beos.h>
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_CAPI)
  #include <botan/es_capi.h>
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_WIN32)
  #include <botan/es_win32.h>
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_FTW)
  #include <botan/es_ftw.h>
#endif

namespace Botan {

/*************************************************
* Default fast poll for EntropySources           *
*************************************************/
u32bit EntropySource::fast_poll(byte buf[], u32bit len)
   {
   return this->slow_poll(buf, len);
   }

/*************************************************
* Get a single random byte                       *
*************************************************/
byte RandomNumberGenerator::next_byte()
   {
   byte out;
   this->randomize(&out, 1);
   return out;
   }

/*************************************************
* Create and seed a new RNG object               *
*************************************************/
RandomNumberGenerator* RandomNumberGenerator::make_rng()
   {
#if defined(BOTAN_HAS_RANDPOOL)

   /* Randpool is required for make_rng to work */
   RandomNumberGenerator* rng = new Randpool(get_block_cipher("AES-256"),
                                             get_mac("HMAC(SHA-256)"));


   /* If X9.31 is available, wrap the Randpool algorithm in it */
#if defined(BOTAN_HAS_X931_RNG)
   rng = new ANSI_X931_RNG(get_block_cipher("AES-256"), rng);
#endif

#if defined(BOTAN_HAS_TIMER_HARDWARE)
   rng->add_entropy_source(new Hardware_Timer);
#elif defined(BOTAN_HAS_TIMER_POSIX)
   rng->add_entropy_source(new POSIX_Timer);
#elif defined(BOTAN_HAS_TIMER_UNIX)
   rng->add_entropy_source(new Unix_Timer);
#elif defined(BOTAN_HAS_TIMER_WIN32)
   rng->add_entropy_source(new Win32_Timer);
#else
   rng->add_entropy_source(new Timer);
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_DEVICE)
   rng->add_entropy_source(
      new Device_EntropySource(
         split_on("/dev/random:/dev/srandom:/dev/urandom", ':')
         )
      );
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_EGD)
   rng->add_entropy_source(
      new EGD_EntropySource(split_on("/var/run/egd-pool:/dev/egd-pool", ':'))
      );
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_CAPI)
   rng->add_entropy_source(new Win32_CAPI_EntropySource);
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_WIN32)
   rng->add_entropy_source(new Win32_EntropySource);
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_UNIX)
   rng->add_entropy_source(
      new Unix_EntropySource(split_on("/bin:/sbin:/usr/bin:/usr/sbin", ':'))
      );
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_BEOS)
   rng->add_entropy_source(new BeOS_EntropySource);
#endif

#if defined(BOTAN_HAS_ENTROPY_SRC_FTW)
   rng->add_entropy_source(new FTW_EntropySource("/proc"));
#endif

   return rng;
#endif

   throw Algorithm_Not_Found("RandomNumberGenerator::make_rng - no RNG found");
   }

}
