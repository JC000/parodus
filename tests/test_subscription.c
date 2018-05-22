/**
 *  Copyright 2010-2016 Comcast Cable Communications Management, LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <malloc.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rebar-c.h"

#include <CUnit/Basic.h>

#include "../src/subscription.h"
#include "../src/ParodusInternal.h"


/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
int sendMsgtoRegisteredClients(char *dest,const char **Msg,size_t msgSize)
{
    UNUSED(dest);
    UNUSED(Msg);
    UNUSED(msgSize);
    return 0;
}
/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/

void test_init_subscription()
{
    init_subscription_list();
    assert_non_null(get_global_subscription_list());
}

void test_add_Client_Subscription()
{
    bool status;
    status = add_Client_Subscription("config", "node-change/*");
    assert_true(status);
    status = add_Client_Subscription("iot", "device-status/*");
    assert_true(status);
    status = add_Client_Subscription("config2", "node-change/*");
    assert_true(status);
    status = add_Client_Subscription("iot2", "device-status/*");
    assert_true(status);
}

void test_get_Client_Subscriptions()
{
    cJSON *json;
    json = get_Client_Subscriptions("config");
    assert_non_null(json);
    cJSON_Delete(json);
}

void test_filter_clients_and_send()
{
    wrp_msg_t wrp_msg;
    memset(&wrp_msg, 0, sizeof(wrp_msg_t));
    wrp_msg.msg_type = WRP_MSG_TYPE__EVENT;     
    wrp_msg.u.event.source = strdup("mac:14cfe214266");
    wrp_msg.u.event.dest   = strdup("event:node-change");
    wrp_msg.u.event.payload = NULL;
    wrp_msg.u.event.payload_size = 0;
    filter_clients_and_send(&wrp_msg);
    free(wrp_msg.u.event.source);
    free(wrp_msg.u.event.dest);
}

void test_delete_client_subscriptions()
{
    UserDataCounter_t data;
    memset(&data, 0, sizeof(UserDataCounter_t));
    data.service_name = strdup("config2");
    delete_client_subscriptions(&data);
    assert_true(1 == data.delete_count && 1 == data.hit_count);
    
    data.hit_count = 0;
    data.delete_count = 0;
    delete_client_subscriptions(&data);
    assert_true(0 == data.delete_count && 0 == data.hit_count);
   
    free(data.service_name);
    data.service_name = strdup("foo");
    delete_client_subscriptions(&data);
    assert_true(0 == data.delete_count && 0 == data.hit_count);
    free(data.service_name);
   
    memset(&data, 0, sizeof(UserDataCounter_t));
    data.service_name = strdup("config");
    delete_client_subscriptions(&data);
    assert_true(1 == data.delete_count && 1 == data.hit_count);
    free(data.service_name);
   
    memset(&data, 0, sizeof(UserDataCounter_t));
    data.service_name = strdup("iot2");
    delete_client_subscriptions(&data);
    assert_true(1 == data.delete_count && 1 == data.hit_count);
    free(data.service_name);
    
    memset(&data, 0, sizeof(UserDataCounter_t));
    data.service_name = strdup("iot");
    delete_client_subscriptions(&data);
    assert_true(1 == data.delete_count && 1 == data.hit_count);
    free(data.service_name);
    
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

int main(void)
{
    int ret;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_subscription),
        cmocka_unit_test(test_add_Client_Subscription),
        cmocka_unit_test(test_get_Client_Subscriptions),
        cmocka_unit_test(test_filter_clients_and_send),
        cmocka_unit_test(test_delete_client_subscriptions),
    };

    ret =  cmocka_run_group_tests(tests, NULL, NULL);
    delete_global_subscription_list();

    return ret;
}
