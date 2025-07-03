#include "sai.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

static const char* profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return NULL;
}

static int profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

static service_method_table_t services = {
    profile_get_value,
    profile_get_next_value
};

int main(void)
{
    sai_status_t status = sai_api_initialize(0, &services);
    assert(status == SAI_STATUS_SUCCESS);

    sai_switch_api_t* switch_api;
    status = sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
    assert(status == SAI_STATUS_SUCCESS);

    sai_switch_notification_t notifications;
    status = switch_api->initialize_switch(0, "HW_ID", NULL, &notifications);
    assert(status == SAI_STATUS_SUCCESS);

    sai_object_id_t ports[64];
    sai_attribute_t ports_attr;
    ports_attr.id = SAI_SWITCH_ATTR_PORT_LIST;
    ports_attr.value.objlist.list = ports;
    ports_attr.value.objlist.count = 64;
    status = switch_api->get_switch_attribute(1, &ports_attr);
    assert(status == SAI_STATUS_SUCCESS);

    for (uint32_t i = 0; i < ports_attr.value.objlist.count; i++) {
        printf("Port #%" PRIu32 " OID: 0x%" PRIX64 "\n", i, ports[i]);
    }

    sai_lag_api_t* lag_api;
    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    assert(status == SAI_STATUS_SUCCESS);

    // Create LAG#1
    sai_object_id_t lag1;
    status = lag_api->create_lag(&lag1, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    // Create LAG_MEMBER#1 {LAG_ID:LAG#1, PORT_ID:PORT#1}
    sai_object_id_t lag_member1;
    sai_attribute_t lag_member1_attrs[2];
    lag_member1_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member1_attrs[0].value.oid = lag1;
    lag_member1_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member1_attrs[1].value.oid = ports[0];
    status = lag_api->create_lag_member(&lag_member1, 2, lag_member1_attrs);
    assert(status == SAI_STATUS_SUCCESS);

    // Create LAG_MEMBER#2 {LAG_ID:LAG#1, PORT_ID:PORT#2}
    sai_object_id_t lag_member2;
    sai_attribute_t lag_member2_attrs[2];
    lag_member2_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member2_attrs[0].value.oid = lag1;
    lag_member2_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member2_attrs[1].value.oid = ports[1];
    status = lag_api->create_lag_member(&lag_member2, 2, lag_member2_attrs);
    assert(status == SAI_STATUS_SUCCESS);

    // Create LAG#2
    sai_object_id_t lag2;
    status = lag_api->create_lag(&lag2, 0, NULL);
    assert(status == SAI_STATUS_SUCCESS);

    // Create LAG_MEMBER#3 {LAG_ID:LAG#2, PORT_ID:PORT#3}
    sai_object_id_t lag_member3;
    sai_attribute_t lag_member3_attrs[2];
    lag_member3_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member3_attrs[0].value.oid = lag2;
    lag_member3_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member3_attrs[1].value.oid = ports[2];
    status = lag_api->create_lag_member(&lag_member3, 2, lag_member3_attrs);
    assert(status == SAI_STATUS_SUCCESS);

    // Create LAG_MEMBER#4 {LAG_ID:LAG#2, PORT_ID:PORT#4}
    sai_object_id_t lag_member4;
    sai_attribute_t lag_member4_attrs[2];
    lag_member4_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_member4_attrs[0].value.oid = lag2;
    lag_member4_attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_member4_attrs[1].value.oid = ports[3];
    status = lag_api->create_lag_member(&lag_member4, 2, lag_member4_attrs);
    assert(status == SAI_STATUS_SUCCESS);

    // Get LAG#1 PORT_LIST [Expected: (PORT#1, PORT#2)]
    sai_object_id_t lag1_ports[2];
    sai_attribute_t lag1_ports_attr;
    lag1_ports_attr.id = SAI_LAG_ATTR_PORT_LIST;
    lag1_ports_attr.value.objlist.list = lag1_ports;
    lag1_ports_attr.value.objlist.count = 2;
    status = lag_api->get_lag_attribute(lag1, 1, &lag1_ports_attr);
    assert(status == SAI_STATUS_SUCCESS);
    assert(lag1_ports_attr.value.objlist.count == 2);
    assert(lag1_ports_attr.value.objlist.list[0] == ports[0]);
    assert(lag1_ports_attr.value.objlist.list[1] == ports[1]);

    // Get LAG#2 PORT_LIST [Expected: (PORT#3, PORT#4)]
    sai_object_id_t lag2_ports[2];
    sai_attribute_t lag2_ports_attr;
    lag2_ports_attr.id = SAI_LAG_ATTR_PORT_LIST;
    lag2_ports_attr.value.objlist.list = lag2_ports;
    lag2_ports_attr.value.objlist.count = 2;
    status = lag_api->get_lag_attribute(lag2, 1, &lag2_ports_attr);
    assert(status == SAI_STATUS_SUCCESS);
    assert(lag2_ports_attr.value.objlist.count == 2);
    assert(lag2_ports_attr.value.objlist.list[0] == ports[2]);
    assert(lag2_ports_attr.value.objlist.list[1] == ports[3]);

    // Get LAG_MEMBER#1 LAG_ID [Expected: LAG#1]
    sai_attribute_t lag_member1_lag_id_attr;
    lag_member1_lag_id_attr.id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    status = lag_api->get_lag_member_attribute(lag_member1, 1, &lag_member1_lag_id_attr);
    assert(status == SAI_STATUS_SUCCESS);
    assert(lag_member1_lag_id_attr.value.oid == lag1);

    // Get LAG_MEMBER#3 PORT_ID [Expected: PORT#3]
    sai_attribute_t lag_member3_port_id_attr;
    lag_member3_port_id_attr.id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    status = lag_api->get_lag_member_attribute(lag_member3, 1, &lag_member3_port_id_attr);
    assert(status == SAI_STATUS_SUCCESS);
    assert(lag_member3_port_id_attr.value.oid == ports[2]);

    // Remove LAG_MEMBER#2
    status = lag_api->remove_lag_member(lag_member2);
    assert(status == SAI_STATUS_SUCCESS);

    // Get LAG#1 PORT_LIST [Expected: (PORT#1)]
    lag1_ports_attr.value.objlist.count = 2;
    status = lag_api->get_lag_attribute(lag1, 1, &lag1_ports_attr);
    assert(status == SAI_STATUS_SUCCESS);
    assert(lag1_ports_attr.value.objlist.count == 1);
    assert(lag1_ports_attr.value.objlist.list[0] == ports[0]);

    // Remove LAG_MEMBER#3
    status = lag_api->remove_lag_member(lag_member3);
    assert(status == SAI_STATUS_SUCCESS);

    // Get LAG#2 PORT_LIST [Expected: (PORT#4)]
    lag2_ports_attr.value.objlist.count = 2;
    status = lag_api->get_lag_attribute(lag2, 1, &lag2_ports_attr);
    assert(status == SAI_STATUS_SUCCESS);
    assert(lag2_ports_attr.value.objlist.count == 1);
    assert(lag2_ports_attr.value.objlist.list[0] == ports[3]);

    // Remove LAG_MEMBER#1
    status = lag_api->remove_lag_member(lag_member1);
    assert(status == SAI_STATUS_SUCCESS);

    // Remove LAG_MEMBER#4
    status = lag_api->remove_lag_member(lag_member4);
    assert(status == SAI_STATUS_SUCCESS);

    // Remove LAG#2
    status = lag_api->remove_lag(lag2);
    assert(status == SAI_STATUS_SUCCESS);

    // Remove LAG#1
    status = lag_api->remove_lag(lag1);
    assert(status == SAI_STATUS_SUCCESS);

    switch_api->shutdown_switch(false);
    status = sai_api_uninitialize();
    assert(status == SAI_STATUS_SUCCESS);

    return 0;
}
