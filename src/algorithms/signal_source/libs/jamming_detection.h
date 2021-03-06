/*!
 * \file 
 * \brief  Interface of a GNU Radio block that sends a STOP message to the
 * control queue right after a specific number of samples have passed through it.
 * \author Javier Arribas, 2018. jarribas(at)cttc.es
 * \author Carlos Aviles, 2010. carlos.avilesr(at)googlemail.com
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2019  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * -------------------------------------------------------------------------
 */


#ifndef GNSS_SDR_GNSS_SDR_JAM_H
#define GNSS_SDR_GNSS_SDR_JAM_H

#include "concurrent_queue.h"
#if GNURADIO_USES_STD_POINTERS
#include <memory>
#else
#include <boost/shared_ptr.hpp>
#endif
#include <gnuradio/sync_block.h>  // for sync_block
#include <gnuradio/types.h>       // for gr_vector_const_void_star
#include <pmt/pmt.h>
#include <cstddef>  // for size_t
#include <cstdint>
#include <memory>

#include <gnuradio/fft/fft.h>
#include <gnuradio/fft/fft_shift.h>

// #define CHUNK_SIZE (2048*8*2) // ~1023 MS/s/32768=30~Hz/bin
#define CHUNK_SIZE (8192*64) // cf Matlab
#define NORM_THRESHOLD (0.08)

class Gnss_Jamming_Protect;
#if GNURADIO_USES_STD_POINTERS
std::shared_ptr<Gnss_Jamming_Protect> gnss_sdr_make_jamm(
    float threshold, int averages);
#else
boost::shared_ptr<Gnss_Jamming_Protect> gnss_sdr_make_jamm(
    float threshold, int averages);
#endif

/*!
 * \brief Implementation of a GNU Radio block that sends a STOP message to the
 * control queue right after a specific number of samples have passed through it.
 */
class Gnss_Jamming_Protect : public gr::sync_block
{
public:
    int work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items);

private:
#if GNURADIO_USES_STD_POINTERS
    friend std::shared_ptr<Gnss_Jamming_Protect> gnss_sdr_make_jamm(
           float threshold, int averages);
#else
    friend boost::shared_ptr<Gnss_Jamming_Protect> gnss_sdr_make_jamm(
           float threshold, int averages);
#endif

    Gnss_Jamming_Protect(float threshold, int averages);
    gr::fft::fft_complex* plan = new gr::fft::fft_complex(CHUNK_SIZE, true);
    gr::fft::fft_complex* iplan= new gr::fft::fft_complex(CHUNK_SIZE, false); // forward = false
    gr_complex bufout0[CHUNK_SIZE];
    gr_complex bufout[CHUNK_SIZE];
    gr_complex processed_output[CHUNK_SIZE];
    gr_complex weight_;
    gr_complex weight_avg_;
    float d_threshold;
    int d_averages;
    int jamming_memory_;
    int avg_index_;
    uint64_t d_ncopied_items;
    Concurrent_Queue<pmt::pmt_t>* d_queue;
};

#endif  // GNSS_SDR_GNSS_SDR_JAM_H
