#ifndef HAVE_QEMU_THUNK_OCIDL_H
#define HAVE_QEMU_THUNK_OCIDL_H

struct qemu_PROPBAG2
{
    DWORD dwType;
    VARTYPE vt;
    CLIPFORMAT cfType;
    DWORD dwHint;
    qemu_ptr pstrName;
    CLSID clsid;
};

static inline void PROPBAG2_h2g(struct qemu_PROPBAG2 *guest, const PROPBAG2 *host)
{
    guest->dwType = host->dwType;
    guest->vt = host->vt;
    guest->cfType = host->cfType;
    guest->dwHint = host->dwHint;
    guest->pstrName = (ULONG_PTR)host->pstrName;
    guest->clsid = host->clsid;
}

static inline void PROPBAG2_g2h(PROPBAG2 *host, const struct qemu_PROPBAG2 *guest)
{
    host->dwType = guest->dwType;
    host->vt = guest->vt;
    host->cfType = guest->cfType;
    host->dwHint = guest->dwHint;
    host->pstrName = (LPOLESTR)(ULONG_PTR)guest->pstrName;
    host->clsid = guest->clsid;
}

#endif
