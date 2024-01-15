/*
 * Copyright (c) 2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_INFO_VERSAL_NET_H_
#define PLATFORM_INFO_VERSAL_NET_H_

#ifndef IPI_IRQ_VECT_ID
#define IPI_IRQ_VECT_ID     90
#endif /* !IPI_IRQ_VECT_ID */

#ifndef POLL_BASE_ADDR
#define POLL_BASE_ADDR       0xEB340000 /* IPI base address*/
#endif /* !POLL_BASE_ADDR */

#ifndef IPI_CHN_BITMASK
#define IPI_CHN_BITMASK     0x0000020
#endif /* !IPI_CHN_BITMASK */

#endif /* PLATFORM_INFO_VERSAL_NET_H_ */
