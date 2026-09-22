#ifndef REMAPPINGBRIDGE_UI_CORE_UIC_05_V1_C_BINDING_H
#define REMAPPINGBRIDGE_UI_CORE_UIC_05_V1_C_BINDING_H

/*
 * UI <-> Core Contract v1.0.0 released C binding.
 *
 * Status: RELEASED v1.0.0 / IMMUTABLE.
 * Semantic source: contract.md
 *
 * This header is the immutable v1.0.0 in-process C binding. Semantic/API
 * changes require a new contract release.
 *
 * The structs below are in-process C ABI objects only. They are NOT a wire,
 * file, flash, IPC, or persistence serialization format and MUST NOT be
 * persisted/transmitted by memcpy. Linked components must use a compatible
 * platform C ABI and this same candidate/released header.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MOUSE_UIC_V1_CONTRACT_MAJOR UINT32_C(1)
#define MOUSE_UIC_V1_CONTRACT_MINOR UINT32_C(0)

#define MOUSE_UIC_V1_MAX_SAVED_MICE UINT32_C(16)
#define MOUSE_UIC_V1_MAX_MOUSE_NAME_UTF8_BYTES UINT32_C(63)
#define MOUSE_UIC_V1_MAX_DIAGNOSTIC_CODE_UTF8_BYTES UINT32_C(31)
#define MOUSE_UIC_V1_MAX_DIAGNOSTIC_MESSAGE_UTF8_BYTES UINT32_C(127)
#define MOUSE_UIC_V1_CUSTOM_SOURCE_COUNT UINT32_C(5)

typedef uint64_t mouse_uic_v1_mouse_id_t;
typedef uint64_t mouse_uic_v1_intent_id_t;
typedef uint64_t mouse_uic_v1_activity_id_t;
typedef uint64_t mouse_uic_v1_revision_t;
typedef uint64_t mouse_uic_v1_notification_seq_t;
typedef uint64_t mouse_uic_v1_capability_mask_t;

#define MOUSE_UIC_V1_ID_NONE UINT64_C(0)

/* Capability bits. Unknown later-minor optional bits are ignored by v1.0 consumers. */
#define MOUSE_UIC_V1_CAP_FIRST_DISCOVERY      (UINT64_C(1) << 0)
#define MOUSE_UIC_V1_CAP_SAVED_RECONNECT      (UINT64_C(1) << 1)
#define MOUSE_UIC_V1_CAP_PAIR_NEW             (UINT64_C(1) << 2)
#define MOUSE_UIC_V1_CAP_PROFILE_PASSTHROUGH  (UINT64_C(1) << 3)
#define MOUSE_UIC_V1_CAP_PROFILE_STANDARD     (UINT64_C(1) << 4)
#define MOUSE_UIC_V1_CAP_PROFILE_ESCAPE       (UINT64_C(1) << 5)
#define MOUSE_UIC_V1_CAP_PROFILE_CUSTOM       (UINT64_C(1) << 6)
#define MOUSE_UIC_V1_CAP_REMOVE_MOUSE         (UINT64_C(1) << 7)
#define MOUSE_UIC_V1_CAP_ESCAPE_OUTPUT        (UINT64_C(1) << 8)

#define MOUSE_UIC_V1_REQUIRED_CAPABILITIES (     MOUSE_UIC_V1_CAP_FIRST_DISCOVERY |     MOUSE_UIC_V1_CAP_SAVED_RECONNECT |     MOUSE_UIC_V1_CAP_PAIR_NEW |     MOUSE_UIC_V1_CAP_PROFILE_PASSTHROUGH |     MOUSE_UIC_V1_CAP_PROFILE_STANDARD |     MOUSE_UIC_V1_CAP_PROFILE_ESCAPE |     MOUSE_UIC_V1_CAP_PROFILE_CUSTOM |     MOUSE_UIC_V1_CAP_REMOVE_MOUSE |     MOUSE_UIC_V1_CAP_ESCAPE_OUTPUT)

/* All public tagged-value domains use uint32_t for deterministic representation. */
typedef uint32_t mouse_uic_v1_profile_t;
#define MOUSE_UIC_V1_PROFILE_INVALID      UINT32_C(0)
#define MOUSE_UIC_V1_PROFILE_PASSTHROUGH  UINT32_C(1)
#define MOUSE_UIC_V1_PROFILE_STANDARD     UINT32_C(2)
#define MOUSE_UIC_V1_PROFILE_ESCAPE       UINT32_C(3)
#define MOUSE_UIC_V1_PROFILE_CUSTOM       UINT32_C(4)

/*
 * C array order for mouse_uic_v1_custom_mapping_t::source_to_target.
 * These are indexes, not target values.
 */
typedef uint32_t mouse_uic_v1_custom_source_index_t;
#define MOUSE_UIC_V1_SOURCE_LEFT_INDEX      UINT32_C(0)
#define MOUSE_UIC_V1_SOURCE_RIGHT_INDEX     UINT32_C(1)
#define MOUSE_UIC_V1_SOURCE_MIDDLE_INDEX    UINT32_C(2)
#define MOUSE_UIC_V1_SOURCE_FORWARD_INDEX   UINT32_C(3)
#define MOUSE_UIC_V1_SOURCE_BACKWARD_INDEX  UINT32_C(4)

typedef uint32_t mouse_uic_v1_custom_target_t;
#define MOUSE_UIC_V1_TARGET_INVALID   UINT32_C(0)
#define MOUSE_UIC_V1_TARGET_LEFT      UINT32_C(1)
#define MOUSE_UIC_V1_TARGET_RIGHT     UINT32_C(2)
#define MOUSE_UIC_V1_TARGET_MIDDLE    UINT32_C(3)
#define MOUSE_UIC_V1_TARGET_ESCAPE    UINT32_C(4)
#define MOUSE_UIC_V1_TARGET_FORWARD   UINT32_C(5)
#define MOUSE_UIC_V1_TARGET_BACKWARD  UINT32_C(6)

typedef uint32_t mouse_uic_v1_search_purpose_t;
#define MOUSE_UIC_V1_SEARCH_PURPOSE_INVALID   UINT32_C(0)
#define MOUSE_UIC_V1_SEARCH_PURPOSE_FIRST     UINT32_C(1)
#define MOUSE_UIC_V1_SEARCH_PURPOSE_SAVED     UINT32_C(2)
#define MOUSE_UIC_V1_SEARCH_PURPOSE_PAIR_NEW  UINT32_C(3)

typedef uint32_t mouse_uic_v1_search_status_t;
#define MOUSE_UIC_V1_SEARCH_STATUS_INVALID    UINT32_C(0)
#define MOUSE_UIC_V1_SEARCH_STATUS_RUNNING    UINT32_C(1)
#define MOUSE_UIC_V1_SEARCH_STATUS_FOUND      UINT32_C(2)
#define MOUSE_UIC_V1_SEARCH_STATUS_TIMED_OUT  UINT32_C(3)
#define MOUSE_UIC_V1_SEARCH_STATUS_FAILED     UINT32_C(4)
#define MOUSE_UIC_V1_SEARCH_STATUS_CANCELLED  UINT32_C(5)

typedef uint32_t mouse_uic_v1_operation_kind_t;
#define MOUSE_UIC_V1_OPERATION_INVALID        UINT32_C(0)
#define MOUSE_UIC_V1_OPERATION_PROFILE_APPLY  UINT32_C(1)
#define MOUSE_UIC_V1_OPERATION_CUSTOM_APPLY   UINT32_C(2)
#define MOUSE_UIC_V1_OPERATION_REMOVE         UINT32_C(3)
#define MOUSE_UIC_V1_OPERATION_HANDOFF        UINT32_C(4)

typedef uint32_t mouse_uic_v1_operation_status_t;
#define MOUSE_UIC_V1_OPERATION_STATUS_INVALID    UINT32_C(0)
#define MOUSE_UIC_V1_OPERATION_STATUS_PENDING    UINT32_C(1)
#define MOUSE_UIC_V1_OPERATION_STATUS_SUCCEEDED  UINT32_C(2)
#define MOUSE_UIC_V1_OPERATION_STATUS_FAILED     UINT32_C(3)
#define MOUSE_UIC_V1_OPERATION_STATUS_CANCELLED  UINT32_C(4)

typedef uint32_t mouse_uic_v1_intent_kind_t;
#define MOUSE_UIC_V1_INTENT_INVALID             UINT32_C(0)
#define MOUSE_UIC_V1_INTENT_START_FIRST_SEARCH  UINT32_C(1)
#define MOUSE_UIC_V1_INTENT_START_SAVED_SEARCH  UINT32_C(2)
#define MOUSE_UIC_V1_INTENT_START_PAIR_NEW      UINT32_C(3)
#define MOUSE_UIC_V1_INTENT_CANCEL_ACTIVITY     UINT32_C(4)
#define MOUSE_UIC_V1_INTENT_APPLY_PROFILE       UINT32_C(5)
#define MOUSE_UIC_V1_INTENT_APPLY_CUSTOM        UINT32_C(6)
#define MOUSE_UIC_V1_INTENT_REMOVE_MOUSE        UINT32_C(7)

typedef uint32_t mouse_uic_v1_submission_disposition_t;
#define MOUSE_UIC_V1_SUBMISSION_INVALID   UINT32_C(0)
#define MOUSE_UIC_V1_SUBMISSION_ACCEPTED  UINT32_C(1)
#define MOUSE_UIC_V1_SUBMISSION_REPLAY    UINT32_C(2)
#define MOUSE_UIC_V1_SUBMISSION_REJECTED  UINT32_C(3)

typedef uint32_t mouse_uic_v1_activity_terminal_status_t;
#define MOUSE_UIC_V1_ACTIVITY_TERMINAL_INVALID    UINT32_C(0)
#define MOUSE_UIC_V1_ACTIVITY_TERMINAL_SUCCEEDED  UINT32_C(1)
#define MOUSE_UIC_V1_ACTIVITY_TERMINAL_TIMED_OUT  UINT32_C(2)
#define MOUSE_UIC_V1_ACTIVITY_TERMINAL_FAILED     UINT32_C(3)
#define MOUSE_UIC_V1_ACTIVITY_TERMINAL_CANCELLED  UINT32_C(4)

typedef uint32_t mouse_uic_v1_notification_kind_t;
#define MOUSE_UIC_V1_NOTIFICATION_INVALID             UINT32_C(0)
#define MOUSE_UIC_V1_NOTIFICATION_ACTIVITY_RESULT     UINT32_C(1)
#define MOUSE_UIC_V1_NOTIFICATION_CONNECTION_CHANGED  UINT32_C(2)

typedef uint32_t mouse_uic_v1_connection_reason_t;
#define MOUSE_UIC_V1_CONNECTION_REASON_INVALID              UINT32_C(0)
#define MOUSE_UIC_V1_CONNECTION_REASON_PHYSICAL_DISCONNECT  UINT32_C(1)
#define MOUSE_UIC_V1_CONNECTION_REASON_PHYSICAL_CONNECT     UINT32_C(2)
#define MOUSE_UIC_V1_CONNECTION_REASON_SEARCH_RECONNECT     UINT32_C(3)
#define MOUSE_UIC_V1_CONNECTION_REASON_FIRST_ACCEPT         UINT32_C(4)
#define MOUSE_UIC_V1_CONNECTION_REASON_HANDOFF              UINT32_C(5)
#define MOUSE_UIC_V1_CONNECTION_REASON_REMOVE_RELEASE       UINT32_C(6)

typedef uint32_t mouse_uic_v1_error_category_t;
#define MOUSE_UIC_V1_ERROR_NONE                   UINT32_C(0)
#define MOUSE_UIC_V1_ERROR_INVALID_REQUEST        UINT32_C(1)
#define MOUSE_UIC_V1_ERROR_CONFLICT               UINT32_C(2)
#define MOUSE_UIC_V1_ERROR_STALE_STATE            UINT32_C(3)
#define MOUSE_UIC_V1_ERROR_LIMIT_REACHED          UINT32_C(4)
#define MOUSE_UIC_V1_ERROR_UNSUPPORTED            UINT32_C(5)
#define MOUSE_UIC_V1_ERROR_INCOMPATIBLE_CONTRACT  UINT32_C(6)
#define MOUSE_UIC_V1_ERROR_AUTHORITY_LOST         UINT32_C(7)
#define MOUSE_UIC_V1_ERROR_TEMPORARY_UNAVAILABLE  UINT32_C(8)
#define MOUSE_UIC_V1_ERROR_PERSISTENCE_FAILURE    UINT32_C(9)
#define MOUSE_UIC_V1_ERROR_INTERNAL_FAILURE       UINT32_C(10)

typedef uint32_t mouse_uic_v1_retryability_t;
#define MOUSE_UIC_V1_RETRY_INVALID             UINT32_C(0)
#define MOUSE_UIC_V1_RETRY_NOT_RETRYABLE       UINT32_C(1)
#define MOUSE_UIC_V1_RETRY_AFTER_REFRESH       UINT32_C(2)
#define MOUSE_UIC_V1_RETRY_AFTER_STATE_CHANGE  UINT32_C(3)
#define MOUSE_UIC_V1_RETRY_LATER               UINT32_C(4)

typedef uint32_t mouse_uic_v1_error_visibility_t;
#define MOUSE_UIC_V1_VISIBILITY_INVALID               UINT32_C(0)
#define MOUSE_UIC_V1_VISIBILITY_DIAGNOSTIC_ONLY       UINT32_C(1)
#define MOUSE_UIC_V1_VISIBILITY_GENERIC_USER_FAILURE  UINT32_C(2)
#define MOUSE_UIC_V1_VISIBILITY_USER_ACTIONABLE       UINT32_C(3)

typedef uint32_t mouse_uic_v1_snapshot_read_kind_t;
#define MOUSE_UIC_V1_SNAPSHOT_READ_INVALID      UINT32_C(0)
#define MOUSE_UIC_V1_SNAPSHOT_READ_SNAPSHOT     UINT32_C(1)
#define MOUSE_UIC_V1_SNAPSHOT_READ_UNAVAILABLE  UINT32_C(2)

typedef uint32_t mouse_uic_v1_call_status_t;
#define MOUSE_UIC_V1_CALL_OK                    UINT32_C(0)
#define MOUSE_UIC_V1_CALL_EMPTY                 UINT32_C(1)
#define MOUSE_UIC_V1_CALL_INVALID_ARGUMENT      UINT32_C(2)
#define MOUSE_UIC_V1_CALL_STRUCT_SIZE_MISMATCH  UINT32_C(3)

typedef struct {
    uint8_t length;
    uint8_t bytes[MOUSE_UIC_V1_MAX_MOUSE_NAME_UTF8_BYTES];
} mouse_uic_v1_mouse_name_t;

typedef struct {
    uint8_t length;
    uint8_t bytes[MOUSE_UIC_V1_MAX_DIAGNOSTIC_CODE_UTF8_BYTES];
} mouse_uic_v1_diagnostic_code_t;

typedef struct {
    uint8_t length;
    uint8_t bytes[MOUSE_UIC_V1_MAX_DIAGNOSTIC_MESSAGE_UTF8_BYTES];
} mouse_uic_v1_diagnostic_message_t;

typedef struct {
    mouse_uic_v1_custom_target_t source_to_target[MOUSE_UIC_V1_CUSTOM_SOURCE_COUNT];
} mouse_uic_v1_custom_mapping_t;

typedef struct {
    mouse_uic_v1_error_category_t category;
    mouse_uic_v1_retryability_t retryability;
    mouse_uic_v1_error_visibility_t visibility;
    uint8_t has_diagnostic_code;
    uint8_t has_diagnostic_message;
    uint8_t reserved_u8[6];
    mouse_uic_v1_diagnostic_code_t diagnostic_code;
    mouse_uic_v1_diagnostic_message_t diagnostic_message;
    uint64_t reserved[2];
} mouse_uic_v1_error_t;

typedef struct {
    mouse_uic_v1_mouse_id_t mouse_id;
    mouse_uic_v1_mouse_name_t name;
    mouse_uic_v1_profile_t confirmed_profile;
    uint32_t reserved_u32;
    uint64_t reserved[2];
} mouse_uic_v1_saved_mouse_t;

typedef struct {
    mouse_uic_v1_mouse_id_t mouse_id;
    mouse_uic_v1_mouse_name_t name;
    uint8_t already_saved;
    uint8_t reserved_u8[7];
    uint64_t reserved[2];
} mouse_uic_v1_candidate_mouse_t;

typedef struct {
    uint8_t present;
    uint8_t has_candidate;
    uint8_t has_error;
    uint8_t reserved_u8;
    mouse_uic_v1_search_purpose_t purpose;
    mouse_uic_v1_search_status_t status;
    mouse_uic_v1_activity_id_t activity_id;
    mouse_uic_v1_intent_id_t origin_intent_id;
    mouse_uic_v1_candidate_mouse_t candidate;
    mouse_uic_v1_error_t error;
    uint64_t reserved[2];
} mouse_uic_v1_search_state_t;

typedef struct {
    uint8_t present;
    uint8_t has_requested_profile;
    uint8_t has_requested_custom;
    uint8_t has_error;
    mouse_uic_v1_operation_kind_t kind;
    mouse_uic_v1_operation_status_t status;
    mouse_uic_v1_activity_id_t activity_id;
    mouse_uic_v1_intent_id_t origin_intent_id;
    mouse_uic_v1_mouse_id_t target_mouse_id;
    mouse_uic_v1_profile_t requested_profile;
    uint32_t reserved_u32;
    mouse_uic_v1_custom_mapping_t requested_custom;
    mouse_uic_v1_error_t error;
    uint64_t reserved[2];
} mouse_uic_v1_operation_state_t;

typedef struct {
    uint32_t struct_size;
    uint32_t reserved_u32;
    mouse_uic_v1_revision_t revision;
    uint32_t saved_count;
    uint8_t has_current_mouse;
    uint8_t reserved_u8[3];
    mouse_uic_v1_mouse_id_t current_mouse_id;
    mouse_uic_v1_saved_mouse_t saved_mice[MOUSE_UIC_V1_MAX_SAVED_MICE];
    mouse_uic_v1_custom_mapping_t custom_confirmed;
    mouse_uic_v1_search_state_t search;
    mouse_uic_v1_operation_state_t operation;
    uint64_t reserved[4];
} mouse_uic_v1_snapshot_t;

typedef struct {
    uint32_t struct_size;
    mouse_uic_v1_snapshot_read_kind_t kind;
    mouse_uic_v1_snapshot_t snapshot;
    mouse_uic_v1_error_t error;
    uint64_t reserved[4];
} mouse_uic_v1_snapshot_read_result_t;

typedef struct {
    uint32_t struct_size;
    uint32_t contract_major;
    uint32_t contract_minor;
    uint32_t max_saved_mice;
    uint32_t max_mouse_name_utf8_bytes;
    uint32_t reserved_u32;
    mouse_uic_v1_capability_mask_t capabilities;
    uint64_t reserved[4];
} mouse_uic_v1_contract_descriptor_t;

typedef struct {
    mouse_uic_v1_activity_id_t target_activity_id;
} mouse_uic_v1_cancel_activity_payload_t;

typedef struct {
    mouse_uic_v1_mouse_id_t target_mouse_id;
    mouse_uic_v1_profile_t profile;
    uint32_t reserved_u32;
} mouse_uic_v1_apply_profile_payload_t;

typedef struct {
    mouse_uic_v1_mouse_id_t target_mouse_id;
    mouse_uic_v1_custom_mapping_t mapping;
} mouse_uic_v1_apply_custom_payload_t;

typedef struct {
    mouse_uic_v1_mouse_id_t target_mouse_id;
} mouse_uic_v1_remove_mouse_payload_t;

typedef union {
    mouse_uic_v1_cancel_activity_payload_t cancel_activity;
    mouse_uic_v1_apply_profile_payload_t apply_profile;
    mouse_uic_v1_apply_custom_payload_t apply_custom;
    mouse_uic_v1_remove_mouse_payload_t remove_mouse;
    uint64_t reserved[8];
} mouse_uic_v1_intent_payload_t;

typedef struct {
    uint32_t struct_size;
    mouse_uic_v1_intent_kind_t kind;
    mouse_uic_v1_intent_id_t intent_id;
    mouse_uic_v1_intent_payload_t payload;
    uint64_t reserved[4];
} mouse_uic_v1_intent_t;

typedef struct {
    uint32_t struct_size;
    mouse_uic_v1_submission_disposition_t disposition;
    uint8_t has_activity_id;
    uint8_t has_error;
    uint8_t reserved_u8[6];
    mouse_uic_v1_activity_id_t activity_id;
    mouse_uic_v1_error_t error;
    uint64_t reserved[4];
} mouse_uic_v1_submission_result_t;

/*
 * ACTIVITY_RESULT class tagging:
 * - search result: search_purpose != INVALID and operation_kind == INVALID
 * - operation result: operation_kind != INVALID and search_purpose == INVALID
 * - search SUCCEEDED carries candidate
 * - FAILED carries error
 * - CANCELLED carries cancel_intent_id
 */
typedef struct {
    mouse_uic_v1_activity_id_t activity_id;
    mouse_uic_v1_intent_id_t origin_intent_id;
    mouse_uic_v1_operation_kind_t operation_kind;
    mouse_uic_v1_search_purpose_t search_purpose;
    mouse_uic_v1_activity_terminal_status_t terminal_status;
    uint32_t reserved_u32;
    mouse_uic_v1_revision_t commit_revision;
    uint8_t has_candidate;
    uint8_t has_cancel_intent_id;
    uint8_t has_error;
    uint8_t reserved_u8[5];
    mouse_uic_v1_candidate_mouse_t candidate;
    mouse_uic_v1_intent_id_t cancel_intent_id;
    mouse_uic_v1_error_t error;
    uint64_t reserved[2];
} mouse_uic_v1_activity_result_t;

typedef struct {
    uint8_t has_previous_current_mouse;
    uint8_t has_current_mouse;
    uint8_t reserved_u8[6];
    mouse_uic_v1_mouse_id_t previous_current_mouse_id;
    mouse_uic_v1_mouse_id_t current_mouse_id;
    mouse_uic_v1_revision_t commit_revision;
    mouse_uic_v1_connection_reason_t reason;
    uint32_t reserved_u32;
    uint64_t reserved[2];
} mouse_uic_v1_connection_changed_t;

typedef union {
    mouse_uic_v1_activity_result_t activity_result;
    mouse_uic_v1_connection_changed_t connection_changed;
    uint64_t reserved[64];
} mouse_uic_v1_notification_payload_t;

typedef struct {
    uint32_t struct_size;
    mouse_uic_v1_notification_kind_t kind;
    mouse_uic_v1_notification_seq_t notification_seq;
    mouse_uic_v1_notification_payload_t payload;
    uint64_t reserved[4];
} mouse_uic_v1_notification_t;

/*
 * Provider object ownership:
 * - object is owned/created/destroyed by Core/integration, not by the UI contract.
 * - caller receives/retains only a pointer supplied by the integration composition root.
 */
typedef struct mouse_uic_v1_provider mouse_uic_v1_provider_t;

/*
 * Threading/lifetime rules:
 * - calls against one provider instance MUST be externally serialized.
 * - API is not reentrant for one provider instance.
 * - caller owns all input/output buffers.
 * - provider MUST NOT retain pointers into caller-owned structs after a call returns.
 * - no allocator/free function crosses this boundary.
 * - caller zero-initializes top-level call structs and sets struct_size.
 * - provider accepts struct_size >= the v1.0 minimum for the same major and ignores
 *   caller trailing bytes it does not understand.
 * - provider writes only fields it knows, zeroes reserved output fields, and sets
 *   nested snapshot.struct_size when returning a SNAPSHOT read result.
 */
mouse_uic_v1_call_status_t mouse_uic_v1_get_descriptor(
    mouse_uic_v1_provider_t *provider,
    mouse_uic_v1_contract_descriptor_t *out_descriptor
);

mouse_uic_v1_call_status_t mouse_uic_v1_read_snapshot(
    mouse_uic_v1_provider_t *provider,
    mouse_uic_v1_snapshot_read_result_t *out_result
);

mouse_uic_v1_call_status_t mouse_uic_v1_submit_intent(
    mouse_uic_v1_provider_t *provider,
    const mouse_uic_v1_intent_t *intent,
    mouse_uic_v1_submission_result_t *out_result
);

mouse_uic_v1_call_status_t mouse_uic_v1_poll_notification(
    mouse_uic_v1_provider_t *provider,
    mouse_uic_v1_notification_t *out_notification
);

#ifdef __cplusplus
}
#endif

#endif /* REMAPPINGBRIDGE_UI_CORE_UIC_05_V1_C_BINDING_H */
