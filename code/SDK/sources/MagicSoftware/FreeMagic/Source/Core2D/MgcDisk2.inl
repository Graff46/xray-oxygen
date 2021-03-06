// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

//----------------------------------------------------------------------------
inline Disk2::Disk2 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector2& Disk2::Center ()
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline const Vector2& Disk2::Center () const
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline Real& Disk2::Radius ()
{
    return m_fRadius;
}
//----------------------------------------------------------------------------
inline const Real& Disk2::Radius () const
{
    return m_fRadius;
}
//----------------------------------------------------------------------------


