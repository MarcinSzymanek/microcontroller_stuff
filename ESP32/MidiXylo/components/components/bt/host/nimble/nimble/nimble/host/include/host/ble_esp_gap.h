/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef H_BLE_ESP_GAP_
#define H_BLE_ESP_GAP_

#ifdef __cplusplus
extern "C" {
#endif


#if MYNEWT_VAL(BLE_HCI_VS)
/**
 * This API is used to clean up residue memory in controller for legacy advertisement
 *
 * @return                0 on success; nonzero on failure.
 */
int ble_gap_clear_legacy_adv(void);

#define BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_ADV_ADDR             0
#define BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_LINK_ID         1
#define BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_BEACON_TYPE     2
#define BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROV_SRV_ADV    3
#define BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROXY_SRV_ADV   4

/**
 * This API  is used to send the Duplicate Exception list VSC to controller
 *
 * @param subcode	  The operation to be done (add/remove/clean)
 *
 * @param type            Exception list type
 *
 * @param value           Device address
 *
 * @cb 		          Registered callback
 *
 * @return                0 on success; nonzero on failure.
 */

int ble_gap_duplicate_exception_list(uint8_t subcode, uint8_t type, uint8_t *value, void *cb);
#endif

#ifdef __cplusplus
}
#endif

#endif
