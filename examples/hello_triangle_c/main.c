#include <stdio.h>
#include "./../../wgpu-bindings/wgpu.h"

#define STAGES_LENGTH        (2)
#define BLEND_STATES_LENGTH  (1)
#define ATTACHMENTS_LENGTH   (1)

WGPUByteArray read_file(const char *name)
{
    FILE *file = fopen(name, "rb");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    unsigned char *bytes = malloc(length);
    fseek(file, 0, SEEK_SET);
    fread(bytes, 1, length, file);
    fclose(file);
    WGPUByteArray ret = {
        .bytes = bytes,
        .length = length,
    };
    return ret;
}

int main()
{
    WGPUInstanceId instance = wgpu_create_instance();
    WGPUAdapterDescriptor adapter_desc = {
        .power_preference = WGPUPowerPreference_LowPower,
    };
    WGPUAdapterId adapter = wgpu_instance_get_adapter(instance, &adapter_desc);
    WGPUDeviceDescriptor device_desc = {
        .extensions = {
            .anisotropic_filtering = false,
        },
    };
    WGPUDeviceId device = wgpu_adapter_create_device(adapter, &device_desc);

    WGPUBindGroupLayoutDescriptor bind_group_layout_desc = {
        .bindings = NULL,
        .bindings_length = 0,
    };
    WGPUBindGroupLayoutId _bind_group_layout = wgpu_device_create_bind_group_layout(device, &bind_group_layout_desc);

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = {
        .bind_group_layouts = NULL,
        .bind_group_layouts_length = 0,
    };
    WGPUPipelineLayoutId layout = wgpu_device_create_pipeline_layout(device, &pipeline_layout_desc);

    WGPUShaderModuleDescriptor vertex_shader_desc = {
        .code = read_file("./../data/hello_triangle.vert.spv"),
    };
    WGPUShaderModuleId vertex_shader = wgpu_device_create_shader_module(device, &vertex_shader_desc);
    WGPUPipelineStageDescriptor vertex_stage = {
        .module = vertex_shader,
        .stage = WGPUShaderStage_Vertex,
        .entry_point = "main",
    };

    WGPUShaderModuleDescriptor fragment_shader_desc = {
        .code = read_file("./../data/hello_triangle.frag.spv"),
    };
    WGPUShaderModuleId fragment_shader = wgpu_device_create_shader_module(device, &fragment_shader_desc);
    WGPUPipelineStageDescriptor fragment_stage = {
        .module = fragment_shader,
        .stage = WGPUShaderStage_Fragment,
        .entry_point = "main",
    };

    WGPUPipelineStageDescriptor stages[STAGES_LENGTH] = { vertex_stage, fragment_stage };

    WGPUBlendDescriptor blend_alpha = {
        .src_factor = WGPUBlendFactor_Zero,
        .dst_factor = WGPUBlendFactor_Zero,
        .operation = WGPUBlendOperation_Add,
    };
    WGPUBlendDescriptor blend_color = {
        .src_factor = WGPUBlendFactor_Zero,
        .dst_factor = WGPUBlendFactor_Zero,
        .operation = WGPUBlendOperation_Add,
    };
    WGPUBlendStateDescriptor blend_state_0_desc = {
        .blend_enabled = false,
        .alpha = blend_alpha,
        .color = blend_color,
        .write_mask = 0,
    };
    WGPUBlendStateId blend_state_0 = wgpu_device_create_blend_state(device, &blend_state_0_desc);
    WGPUBlendStateId blend_state[BLEND_STATES_LENGTH] = { blend_state_0 };

    WGPUStencilStateFaceDescriptor stencil_state_front = {
        .compare = WGPUCompareFunction_Never,
        .stencil_fail_op = WGPUStencilOperation_Keep,
        .depth_fail_op = WGPUStencilOperation_Keep,
        .pass_op = WGPUStencilOperation_Keep,
    };
    WGPUStencilStateFaceDescriptor stencil_state_back = {
        .compare = WGPUCompareFunction_Never,
        .stencil_fail_op = WGPUStencilOperation_Keep,
        .depth_fail_op = WGPUStencilOperation_Keep,
        .pass_op = WGPUStencilOperation_Keep,
    };
    WGPUDepthStencilStateDescriptor depth_stencil_state_desc = {
        .depth_write_enabled = false,
        .depth_compare = WGPUCompareFunction_Never,
        .front = stencil_state_front,
        .back = stencil_state_back,
        .stencil_read_mask = 0,
        .stencil_write_mask = 0,
    };
    WGPUDepthStencilStateId depth_stencil_state = wgpu_device_create_depth_stencil_state(device, &depth_stencil_state_desc);

    WGPUAttachment attachments[ATTACHMENTS_LENGTH] = {
        {
            .format = WGPUTextureFormat_R8g8b8a8Unorm,
            .samples = 1,
        },
    };
    WGPUAttachmentsState attachment_state = {
        .color_attachments = attachments,
        .color_attachments_length = ATTACHMENTS_LENGTH,
        .depth_stencil_attachment = NULL,
    };

    WGPURenderPipelineDescriptor render_pipeline_desc = {
        .layout = layout,
        .stages = stages,
        .stages_length = STAGES_LENGTH,
        .primitive_topology = WGPUPrimitiveTopology_TriangleList,
        .attachments_state = attachment_state,
        .blend_states = blend_state,
        .blend_states_length = BLEND_STATES_LENGTH,
        .depth_stencil_state = depth_stencil_state,
    };

    WGPURenderPipelineId render_pipeline = wgpu_device_create_render_pipeline(device, &render_pipeline_desc);

    WGPUCommandBufferDescriptor cmd_buf_desc = { };
    WGPUCommandBufferId cmd_buf = wgpu_device_create_command_buffer(device, &cmd_buf_desc);
    WGPUQueueId queue = wgpu_device_get_queue(device);
    wgpu_queue_submit(queue, &cmd_buf, 1);

    const uint32_t TEXTURE_WIDTH = 800;
    const uint32_t TEXTURE_HEIGHT = 600;

    WGPUExtent3d texture_size = {
        .width = TEXTURE_WIDTH,
        .height = TEXTURE_HEIGHT,
        .depth = 1,
    };

    const WGPUTextureFormat texture_format = WGPUTextureFormat_R8g8b8a8Unorm;
    const WGPUTextureUsageFlags texture_usage = WGPUTextureUsageFlags_TRANSFER_DST | WGPUTextureUsageFlags_SAMPLED;

    WGPUTextureDescriptor texture_desc = {
        .size = texture_size,
        .array_size = 1,
        .dimension = WGPUTextureDimension_D2,
        .format = texture_format,
        .usage = texture_usage,
    };

    WGPUTextureId texture = wgpu_device_create_texture(device, &texture_desc);

    return 0;
}
