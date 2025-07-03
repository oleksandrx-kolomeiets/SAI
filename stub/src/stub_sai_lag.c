#include "sai.h"
#include "stub_sai.h"

#include <inttypes.h>

#define LAG_MEMBER_DB_SIZE 16

static struct {
    bool used;
    sai_object_id_t port_oid;
    sai_object_id_t lag_oid;
} lag_member_db[LAG_MEMBER_DB_SIZE];

sai_status_t get_lag_attribute(
    _In_ const sai_object_key_t *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t attr_index,
    _Inout_ vendor_cache_t *cache,
    void *arg)
{
    if ((int64_t) arg != SAI_LAG_ATTR_PORT_LIST) {
        return SAI_STATUS_FAILURE;
    }

    uint32_t count = 0;
    for (uint32_t i = 0; i < LAG_MEMBER_DB_SIZE; i++) {
        if (lag_member_db[i].used && lag_member_db[i].lag_oid == key->object_id) {
            if (count < value->objlist.count) {
                value->objlist.list[count] = lag_member_db[i].port_oid;
            }
            count++;
        }
    }

    sai_status_t status = SAI_STATUS_SUCCESS;
    if (count > value->objlist.count) {
        status = SAI_STATUS_BUFFER_OVERFLOW;
    }
    value->objlist.count = count;
    return status;
}

sai_status_t get_lag_member_attribute(
    _In_ const sai_object_key_t *key,
    _Inout_ sai_attribute_value_t *value,
    _In_ uint32_t attr_index,
    _Inout_ vendor_cache_t *cache,
    void *arg)
{
    uint32_t index;
    sai_status_t status = stub_object_to_type(key->object_id, SAI_OBJECT_TYPE_LAG_MEMBER, &index);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    switch ((int64_t) arg) {
        case SAI_LAG_MEMBER_ATTR_LAG_ID:
            value->oid = lag_member_db[index].lag_oid;
            break;
        case SAI_LAG_MEMBER_ATTR_PORT_ID:
            value->oid = lag_member_db[index].port_oid;
            break;
        default:
            return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

static const sai_attribute_entry_t lag_attribs[] = {
    {
        SAI_LAG_ATTR_PORT_LIST,
        false, false, false, true,
        "List of ports in LAG",
        SAI_ATTR_VAL_TYPE_OBJLIST
    },
    {
        END_FUNCTIONALITY_ATTRIBS_ID,
        false, false, false, false,
        "",
        SAI_ATTR_VAL_TYPE_UNDETERMINED
    },
};

static const sai_attribute_entry_t lag_member_attribs[] = {
    {
        SAI_LAG_MEMBER_ATTR_LAG_ID,
        true, true, false, true,
        "LAG ID",
        SAI_ATTR_VAL_TYPE_OID
    },
    {
        SAI_LAG_MEMBER_ATTR_PORT_ID,
        true, true, false, true,
        "PORT ID",
        SAI_ATTR_VAL_TYPE_OID
    },
    {
        END_FUNCTIONALITY_ATTRIBS_ID,
        false, false, false, false,
        "",
        SAI_ATTR_VAL_TYPE_UNDETERMINED
    },
};

static const sai_vendor_attribute_entry_t lag_vendor_attribs[] = {
    {
        SAI_LAG_ATTR_PORT_LIST,
        { false, false, false, true },
        { false, false, false, true },
        get_lag_attribute, (void*) SAI_LAG_ATTR_PORT_LIST,
        NULL, NULL
    },
};

static const sai_vendor_attribute_entry_t lag_member_vendor_attribs[] = {
    {
        SAI_LAG_MEMBER_ATTR_LAG_ID,
        { true, false, false, true },
        { true, false, false, true },
        get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_LAG_ID,
        NULL, NULL
    },
    {
        SAI_LAG_MEMBER_ATTR_PORT_ID,
        { true, false, false, true },
        { true, false, false, true },
        get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_PORT_ID,
        NULL, NULL
    },
};

sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    char list_str[MAX_LIST_VALUE_STR_LEN];
    sai_attr_list_to_str(attr_count, attr_list, lag_attribs, MAX_LIST_VALUE_STR_LEN, list_str);

    static uint32_t next_lag_id = 1;
    sai_status_t status = stub_create_object(SAI_OBJECT_TYPE_LAG, next_lag_id++, lag_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create a LAG OID\n");
        return status;
    }
    printf("CREATE LAG: 0x%" PRIX64 " (%s)\n", *lag_id, list_str);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_remove_lag(
    _In_ sai_object_id_t  lag_id)
{
    printf("REMOVE LAG: 0x%" PRIX64 "\n", lag_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_attribute(
    _In_ sai_object_id_t  lag_id,
    _In_ const sai_attribute_t *attr)
{
    return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t stub_get_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .object_id = lag_id };
    sai_status_t status = sai_get_attributes(&key, NULL, lag_attribs, lag_vendor_attribs, attr_count, attr_list);
    char list_str[MAX_LIST_VALUE_STR_LEN];
    sai_attr_list_to_str(attr_count, attr_list, lag_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("GET LAG ATTRIBUTE: 0x%" PRIX64 " (%s)\n", lag_id, list_str);
    return status;
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    char list_str[MAX_LIST_VALUE_STR_LEN];
    sai_attr_list_to_str(attr_count, attr_list, lag_member_attribs, MAX_LIST_VALUE_STR_LEN, list_str);

    uint32_t lag_member_db_index = LAG_MEMBER_DB_SIZE;
    for (uint32_t i = 0; i < LAG_MEMBER_DB_SIZE; i++) {
        if (!lag_member_db[i].used) {
            lag_member_db_index = i;
            break;
        }
    }

    if (lag_member_db_index >= LAG_MEMBER_DB_SIZE) {
        return SAI_STATUS_FAILURE;
    }
    lag_member_db[lag_member_db_index].used = true;

    uint32_t lag_id_idx;
    const sai_attribute_value_t *lag_id;
    sai_status_t status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_LAG_ID, &lag_id, &lag_id_idx);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    lag_member_db[lag_member_db_index].lag_oid = lag_id->oid;

    uint32_t port_id_idx;
    const sai_attribute_value_t *port_id;
    status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_PORT_ID, &port_id, &port_id_idx);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    lag_member_db[lag_member_db_index].port_oid = port_id->oid;

    status = stub_create_object(SAI_OBJECT_TYPE_LAG_MEMBER, lag_member_db_index, lag_member_id);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Cannot create a LAG MEMBER OID\n");
        return status;
    }

    printf("CREATE LAG MEMBER: 0x%" PRIX64 " (%s)\n", *lag_member_id, list_str);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t lag_member_id)
{
    uint32_t lag_member_db_index;
    sai_status_t status = stub_object_to_type(lag_member_id, SAI_OBJECT_TYPE_LAG_MEMBER, &lag_member_db_index);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }
    lag_member_db[lag_member_db_index].used = false;

    printf("REMOVE LAG MEMBER: 0x%" PRIX64 "\n", lag_member_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_member_attribute(
    _In_ sai_object_id_t  lag_member_id,
    _In_ const sai_attribute_t *attr)
{
    return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t stub_get_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .object_id = lag_member_id };
    sai_status_t status = sai_get_attributes(&key, NULL, lag_member_attribs, lag_member_vendor_attribs, attr_count, attr_list);
    char list_str[MAX_LIST_VALUE_STR_LEN];
    sai_attr_list_to_str(attr_count, attr_list, lag_member_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("GET LAG MEMBER ATTRIBUTE: 0x%" PRIX64 " (%s)\n", lag_member_id, list_str);
    return status;
}

const sai_lag_api_t lag_api = {
    stub_create_lag,
    stub_remove_lag,
    stub_set_lag_attribute,
    stub_get_lag_attribute,
    stub_create_lag_member,
    stub_remove_lag_member,
    stub_set_lag_member_attribute,
    stub_get_lag_member_attribute
};
