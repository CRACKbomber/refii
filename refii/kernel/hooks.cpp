#include <stdafx.h>
#include <kernel/kernel.h>
#include <kernel/function.h>
#include <gpu/video.h>

// Misc Kernel funcs
GUEST_FUNCTION_HOOK(sub_82CC2E48, refii::kernel::SleepEx);
GUEST_FUNCTION_HOOK(sub_822D6D20, refii::kernel::WaitForSingleObjectEx);
GUEST_FUNCTION_HOOK(sub_82CC2670, refii::kernel::XSetThreadProcessor);
GUEST_FUNCTION_HOOK(sub_82B3DAE8, refii::kernel::SetThreadName);
GUEST_FUNCTION_HOOK(sub_8221EA28, refii::kernel::QueryPerformanceCounter);
GUEST_FUNCTION_HOOK(sub_82CC6E78, refii::kernel::QueryPerformanceFrequency);
GUEST_FUNCTION_HOOK(sub_822660B0, refii::kernel::GetTickCount);
GUEST_FUNCTION_HOOK(sub_82CC2440, refii::kernel::GlobalMemoryStatus);

// Heap Functions
#ifdef REFII_HEAP_EMU
    GUEST_FUNCTION_HOOK(sub_822387B8, refii::kernel::RtlAllocateHeap);
    GUEST_FUNCTION_HOOK(sub_82239490, refii::kernel::RtlFreeHeap);
    GUEST_FUNCTION_HOOK(sub_82CC62E0, refii::kernel::RtlReAllocateHeap);
    GUEST_FUNCTION_HOOK(sub_82239808, refii::kernel::RtlSizeHeap);
    GUEST_FUNCTION_HOOK(sub_82CC5D38, refii::kernel::RtlCreateHeap);
    GUEST_FUNCTION_HOOK(sub_82CC4F98, refii::kernel::RtlDestroyHeap);
#else 
    GUEST_FUNCTION_HOOK(sub_822387B8, refii::kernel::RtlAllocateHeap);
    GUEST_FUNCTION_HOOK(sub_82239490, refii::kernel::RtlFreeHeap);
    GUEST_FUNCTION_HOOK(sub_82CC62E0, refii::kernel::RtlReAllocateHeap);
    GUEST_FUNCTION_HOOK(sub_82239808, refii::kernel::RtlSizeHeap);
    GUEST_FUNCTION_ALIAS_STUB(sub_82CC5D38, "RtlCreateHeap");
    GUEST_FUNCTION_ALIAS_STUB(sub_82CC4F98, "RtlDestroyHeap");
    GUEST_FUNCTION_ALIAS_STUB(sub_82CC6BC0, "HeapCreate");
    GUEST_FUNCTION_ALIAS_STUB(sub_82CC6C40, "HeapDestroy");
#endif


GUEST_FUNCTION_HOOK(sub_822386D0, refii::kernel::XAllocMem);
GUEST_FUNCTION_HOOK(sub_822398B0, refii::kernel::XFreeMem);
GUEST_FUNCTION_HOOK(sub_82CC74F8, refii::kernel::VirtualAlloc);
GUEST_FUNCTION_HOOK(sub_82CC7548, refii::kernel::VirtualFree);
// native memory operations
GUEST_FUNCTION_HOOK(sub_82CB0B30, memmove);
GUEST_FUNCTION_HOOK(sub_826BF770, memcpy);
GUEST_FUNCTION_HOOK(sub_826BFCF0, memset);
//GUEST_FUNCTION_HOOK(sub_82170010, OutputDebugStringA);

//
// // sprintf
// PPC_FUNC(sub_82BD4AE8)
//{
//    sub_831B1630(ctx, base);
//}
//

// todo(crack): move this some where more gooder
std::string wchar_to_cstr(wchar_t const* wcstr) {
    auto s = std::mbstate_t();
    auto const target_char_count = std::wcsrtombs(nullptr, &wcstr, 0, &s);
    if (target_char_count == static_cast<std::size_t>(-1)) {
        throw std::logic_error("Illegal byte sequence");
    }

    // +1 because std::string adds a null terminator which isn't part of size
    auto str = std::string(target_char_count, '\0');
    std::wcsrtombs(str.data(), &wcstr, str.size() + 1, &s);
    return str;
}

void GuestDbgPrint(char* msg)
{
    LOG_UTILITY(msg);
}
void GuestDbgPrintW(wchar_t* msg)
{
    LOG_UTILITY(wchar_to_cstr(msg));
}
// OutputDebugStringA --> GuestDbgPrint
GUEST_FUNCTION_HOOK(sub_82170010, GuestDbgPrint);
// OutputDebugStringW --> GuestDbgPrintW
GUEST_FUNCTION_HOOK(sub_82CC7FF0, GuestDbgPrintW);


// stubs

GUEST_FUNCTION_STUB(__imp__vsprintf);
GUEST_FUNCTION_STUB(__imp___vsnprintf);
GUEST_FUNCTION_STUB(__imp__sprintf);
GUEST_FUNCTION_STUB(__imp___snprintf);
GUEST_FUNCTION_STUB(__imp___snwprintf);
GUEST_FUNCTION_STUB(__imp__vswprintf);
GUEST_FUNCTION_STUB(__imp___vscwprintf);
GUEST_FUNCTION_STUB(__imp__swprintf);
// sub_82C87FF0 attempts to init sockets
GUEST_FUNCTION_ALIAS_STUB(sub_82C87FF0, "_unk_init_wsa")

// sub_82CFF790 is attempting to init xbl services
GUEST_FUNCTION_ALIAS_STUB(sub_822EAAE0, "init_xbl")

// XRNM is used for network stuff and is a pain in the ass
GUEST_FUNCTION_ALIAS_STUB(sub_8258C398, "XrnmThreadStartup");

void FixR11CaseIssue(PPCRegister& r11)
{
    // r11.u64 is the base of the guest memory (0x0000000100000000) when it should be 0
    if (r11.u64 == 0x0000000100000000)
    {
        r11.u64 = 0;
    }
}

// D3D Rendering Hooks

GUEST_FUNCTION_HOOK(sub_82BA6990, refii::gpu::InitializeHardwareDevice);

// D3DXFillTexture
//GUEST_FUNCTION_HOOK(sub_, refii::gpu::D3DXFillTexture);

// D3DXFillVolumeTexture
//GUEST_FUNCTION_HOOK(sub_, refii::gpu::D3DXFillVolumeTexture);

// D3DDevice_SetRenderTarget
GUEST_FUNCTION_HOOK(sub_822869A0, refii::gpu::SetRenderTarget);

// D3DDevice_DestroyResource
GUEST_FUNCTION_HOOK(sub_821F65C0, refii::gpu::DestructResource);

// D3DDevice_SetStreamSource
GUEST_FUNCTION_HOOK(sub_821B6C60, refii::gpu::SetStreamSource);

// D3DDevice_SetViewport
GUEST_FUNCTION_HOOK(sub_821F9F58, refii::gpu::SetViewport);
 
// D3DDevice_Clear
GUEST_FUNCTION_HOOK(sub_821E2FE0, refii::gpu::Clear);
 
// D3DDevice_Present
// GUEST_FUNCTION_HOOK(sub_, Video::Present);
 
// D3DDevice_SetResolution
// GUEST_FUNCTION_HOOK(sub_, refii::gpu::SetResolution);
 
// D3DDevice_CreateSurface
GUEST_FUNCTION_HOOK(sub_821F4EC0, refii::gpu::CreateSurface);
 
// D3DSurface_GetDesc
GUEST_FUNCTION_HOOK(sub_821FA070, refii::gpu::GetSurfaceDesc);

// D3DDevice_SetDepthStencilSurface
GUEST_FUNCTION_HOOK(sub_82264590, refii::gpu::SetDepthStencilSurface);

// D3DDevice_CreateTexture
GUEST_FUNCTION_HOOK(sub_82B98B28, refii::gpu::CreateTexture);

// D3DDevice_SetTexture
GUEST_FUNCTION_HOOK(sub_821B6EE0, refii::gpu::SetTexture);

// LockTextureRect
GUEST_FUNCTION_HOOK(sub_, refii::gpu::LockTextureRect);

// UnlockTextureRect
// GUEST_FUNCTION_HOOK(sub_, refii::gpu::UnlockTextureRect);

// D3DDevice_SetScissorRect
GUEST_FUNCTION_HOOK(sub_821F9C08, refii::gpu::SetScissorRect);

// D3DDevice_Resolve
// GUEST_FUNCTION_HOOK(sub_, refii::gpu::StretchRect);

// D3DDevice_SetIndices
GUEST_FUNCTION_HOOK(sub_8219CC30, refii::gpu::SetIndices);

// D3DDevice_CreatePixelShader
GUEST_FUNCTION_HOOK(sub_82B992B0, refii::gpu::CreatePixelShader);

// D3DDevice_SetPixelShader
GUEST_FUNCTION_HOOK(sub_82208BB0, refii::gpu::SetPixelShader);

// D3DDevice_CreateIndexBuffer
// GUEST_FUNCTION_HOOK(sub_, refii::gpu::CreateIndexBuffer);

// D3DIndexBuffer_Lock
GUEST_FUNCTION_HOOK(sub_82196980, refii::gpu::LockIndexBuffer);

// D3DIndexBuffer_Unlock
GUEST_FUNCTION_HOOK(sub_82B9EED0, refii::gpu::UnlockIndexBuffer);

// D3DIndexBuffer_GetDesc
// GUEST_FUNCTION_HOOK(sub_, refii::gpu::GetIndexBufferDesc);

// D3DDevice_CreateVertexShader
GUEST_FUNCTION_HOOK(sub_82B994BC, refii::gpu::CreateVertexShader);

// D3DDevice_SetVertexShader
GUEST_FUNCTION_HOOK(sub_82232510, refii::gpu::SetVertexShader);

// D3DDevice_CreateVertexDeclaration
GUEST_FUNCTION_HOOK(sub_82B99720, refii::gpu::CreateVertexDeclaration);

// D3DDevice_GetVertexDeclaration
// GUEST_FUNCTION_HOOK(sub_, reblue::gpu::GetVertexDeclaration);

// D3DDevice_SetVertexDeclaration
GUEST_FUNCTION_HOOK(sub_822224A8, refii::gpu::SetVertexDeclaration);

// D3DDevice_HashVertexDeclaration
// GUEST_FUNCTION_HOOK(sub_, reblue::gpu::HashVertexDeclaration);

// D3DDevice_CreateVertexBuffer
// GUEST_FUNCTION_HOOK(sub_, refii::gpu::CreateVertexBuffer);

// D3DVertexBuffer_Lock
GUEST_FUNCTION_HOOK(sub_8225F980, refii::gpu::LockVertexBuffer);

// D3DVertexBuffer_Unlock
GUEST_FUNCTION_HOOK(sub_82B9EEC0, refii::gpu::UnlockVertexBuffer);

// D3DVertexBuffer_GetDesc
// GUEST_FUNCTION_HOOK(sub_, reblue::gpu::GetVertexBufferDesc);

// D3DDevice_DrawIndexedVertices
GUEST_FUNCTION_HOOK(sub_8221DFC0, refii::gpu::DrawIndexedPrimitive);

// D3DDevice_DrawVertices
GUEST_FUNCTION_HOOK(sub_8221C3E8, refii::gpu::DrawPrimitive);

// D3DDevice_DrawVerticesUP
GUEST_FUNCTION_HOOK(sub_82265CD8, refii::gpu::DrawPrimitiveUP);

GUEST_FUNCTION_ALIAS_STUB(sub_82BA2270, "D3DDevice_AcquireThreadOwnership");
GUEST_FUNCTION_ALIAS_STUB(sub_82BA22B0, "D3DDevice_ReleaseThreadOwnership");
GUEST_FUNCTION_ALIAS_STUB(sub_82B98248, "D3DDevice_SetGammaRamp");
GUEST_FUNCTION_ALIAS_STUB(sub_8219CD68, "D3DDevice_SetShaderGPRAllocation");
GUEST_FUNCTION_ALIAS_STUB(sub_83024AD8, "D3DXCompileShaderEx");
GUEST_FUNCTION_ALIAS_STUB(sub_822655F0, "D3DDevice_SetPrediction");
GUEST_FUNCTION_ALIAS_STUB(sub_82D168C8, "D3DXFilterTexture");
GUEST_FUNCTION_ALIAS_STUB(sub_82BA2218, "D3DDevice_Release");
