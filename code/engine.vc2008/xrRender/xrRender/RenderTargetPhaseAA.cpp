#include "stdafx.h"

void CRenderTarget::ProcessFXAA()
{
	// Luminance pass
	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Generic_0, s_pp_antialiasing->E[0]);

	// Main pass
#ifdef USE_DX11
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Generic_2, s_pp_antialiasing->E[1]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#else
	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Color, s_pp_antialiasing->E[1]);
#endif
}

void CRenderTarget::ProcessTAA()
{
#ifdef USE_DX11
	// Temporal AA passing
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Generic_2, s_pp_taa->E[0]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#endif
}

void CRenderTarget::ProcessDLAA()
{
	// Pass 0
#ifdef USE_DX11
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Generic_2, s_pp_antialiasing->E[5]);
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#else
	RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_Color, s_pp_antialiasing->E[5]);
#endif
}

void CRenderTarget::ProcessSMAA()
{
	u32 Offset;
	Fvector2 p0, p1;
	float d_Z	= EPS_S;
	float d_W	= 1.0f;
	u32	C		= color_rgba(0, 0, 0, 255);

	float _w	= float(Device.dwWidth);
	float _h	= float(Device.dwHeight);

	// Half-pixel offset (DX9 only)
#ifdef USE_DX11
	p0.set(0.0f, 0.0f);
	p1.set(1.0f, 1.0f);
#else
	p0.set(0.5f / _w, 0.5f / _h);
	p1.set((_w + 0.5f) / _w, (_h + 0.5f) / _h);
#endif

	// Phase 0: edge detection ////////////////////////////////////////////////
	u_setrt				(rt_smaa_edgetex, nullptr, nullptr, nullptr);
	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(TRUE, D3D11_COMPARISON_ALWAYS, 0x1, 0, 0, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP);
	RCache.Clear		(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(EPS,	    _h + EPS, d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(EPS,	    EPS,	  d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(_w + EPS,	_h + EPS, d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(_w + EPS,	EPS,	  d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_pp_antialiasing->E[2]);
	RCache.set_Geometry	(g_combine);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	// Phase 1: blend weights calculation ////////////////////////////////////
	u_setrt				(rt_smaa_blendtex, nullptr, nullptr, nullptr);
	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(TRUE, D3D11_COMPARISON_EQUAL, 0x1, 0, 0, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP);
	RCache.Clear		(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);

	// Fill vertex buffer
	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(EPS,	    _h + EPS, d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(EPS,	    EPS,	  d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(_w + EPS,	_h + EPS, d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(_w + EPS,	EPS,	  d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_pp_antialiasing->E[3]);
	RCache.set_Geometry	(g_combine);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	// Phase 2: neighbour blend //////////////////////////////////////////////
#ifdef USE_DX11
	u_setrt(rt_Generic_2, nullptr, nullptr, nullptr);
#else
	u_setrt(rt_Color, nullptr, nullptr, nullptr);
#endif

	RCache.set_CullMode	(CULL_NONE);
	RCache.set_Stencil	(FALSE);

	// Fill vertex buffer
	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(EPS,	    _h + EPS, d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(EPS,	    EPS,	  d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(_w + EPS,	_h + EPS, d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(_w + EPS,	EPS,	  d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Draw COLOR
	RCache.set_Element	(s_pp_antialiasing->E[4]);
	RCache.set_Geometry	(g_combine);
	RCache.Render		(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

#ifdef USE_DX11
	ref_rt outRT = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
	HW.pContext->CopyResource(outRT->pTexture->surface_get(), rt_Generic_2->pTexture->surface_get());
#endif
}
	
void CRenderTarget::PhaseAA()
{
	switch (ps_r_pp_aa_mode)
	{
	case FXAA: ProcessFXAA(); break;
	case SMAA: ProcessSMAA(); break;
	case DLAA: ProcessDLAA(); break;
	}
	
	if (ps_r_pp_aa_quality > 1)
		ProcessTAA();
}
