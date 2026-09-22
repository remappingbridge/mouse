#include "uic_v1.h"

#include <stddef.h>
#include <stdint.h>

_Static_assert(sizeof(mouse_uic_v1_mouse_id_t) == 8u, "mouse id must be 64-bit");
_Static_assert(sizeof(mouse_uic_v1_intent_id_t) == 8u, "intent id must be 64-bit");
_Static_assert(sizeof(mouse_uic_v1_activity_id_t) == 8u, "activity id must be 64-bit");
_Static_assert(MOUSE_UIC_V1_MAX_SAVED_MICE == 16u, "saved count");
_Static_assert(MOUSE_UIC_V1_MAX_MOUSE_NAME_UTF8_BYTES == 63u, "name bound");
_Static_assert(MOUSE_UIC_V1_CUSTOM_SOURCE_COUNT == 5u, "custom source count");
_Static_assert(
    (MOUSE_UIC_V1_REQUIRED_CAPABILITIES & MOUSE_UIC_V1_CAP_ESCAPE_OUTPUT) != 0u,
    "Escape output must be required"
);

static void compile_shapes(void)
{
    mouse_uic_v1_contract_descriptor_t descriptor = {0};
    mouse_uic_v1_snapshot_read_result_t snapshot_result = {0};
    mouse_uic_v1_intent_t intent = {0};
    mouse_uic_v1_submission_result_t submission = {0};
    mouse_uic_v1_notification_t notification = {0};

    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    snapshot_result.struct_size = (uint32_t)sizeof(snapshot_result);
    snapshot_result.snapshot.struct_size =
        (uint32_t)sizeof(snapshot_result.snapshot);
    intent.struct_size = (uint32_t)sizeof(intent);
    submission.struct_size = (uint32_t)sizeof(submission);
    notification.struct_size = (uint32_t)sizeof(notification);

    intent.intent_id = UINT64_C(1);
    intent.kind = MOUSE_UIC_V1_INTENT_APPLY_PROFILE;
    intent.payload.apply_profile.target_mouse_id = UINT64_C(1);
    intent.payload.apply_profile.profile = MOUSE_UIC_V1_PROFILE_STANDARD;

    (void)descriptor;
    (void)snapshot_result;
    (void)intent;
    (void)submission;
    (void)notification;
}

int main(void)
{
    compile_shapes();
    return 0;
}
