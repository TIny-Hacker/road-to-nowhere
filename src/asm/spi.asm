
    assume adl=1

    section .text

include 'include/ti84pceg.inc'

    public _beginFrame
    public _endFrame
    public _setupSPI

spiValid := 8

;--------------------------------------

macro spi cmd, params&
    ld a, cmd
    call spiCmd
    match any, params
        iterate param, any
            ld a, param
            call spiParam
        end iterate
    end match
end macro

;--------------------------------------

_beginFrame:
    ld a, (ti.mpLcdRis)
    and a, ti.lcdIntVcomp
    jr z, _beginFrame
    ld (ti.mpLcdIcr), a
    spi $B0, $01 ; disable framebuffer copies
    spi $2C
    ret

_endFrame:
    ld a, (ti.mpLcdCurr + 2) ; a = *mpLcdCurr >> 16
    ld hl, (ti.mpLcdCurr + 1) ; hl = *mpLcdCurr >> 8
    sub a, h
    jr nz, _endFrame ; nz ==> lcdCurr may have updated mid-read; retry read
    ld de, -ti.lcdWidth * ti.lcdHeight
    add hl, de
    ld de, (ti.mpLcdBase)
    or a, a
    sbc hl, de
    or a, a
    sbc hl, de
    jr z, .resetVcomp
    ld a, ti.lcdIntVcomp
    ld (ti.mpLcdIcr), a

.loop:
    ld a, (ti.mpLcdRis)
    bit ti.bLcdIntVcomp, a
    jr z, .loop

.resetVcomp:
    ld a, ti.lcdIntVcomp
    ld (ti.mpLcdIcr), a
    spi $B0, $11 ; enable framebuffer copies
    ret

_setupSPI: ; Necessary on Python models
    scf
    ld hl, $2000B
    ld (ti.mpSpiRange + ti.spiCtrl1), hl
    ld hl, $1828
.loop:
    ld (ti.mpSpiRange + ti.spiCtrl0), hl
    ld hl, $0C
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    ccf
    ld hl, $40
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    ld hl, $182B
    jr nc, .loop
    ld hl, $21
    ld (ti.mpSpiRange + ti.spiIntCtrl), hl
    ld hl, $100
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    ret

spiParam:
    scf
    virtual
        jr nc, $
        load .jr_nc : byte from $$
    end virtual
    db .jr_nc

spiCmd:
    or a, a
    ld hl, ti.mpSpiData or spiValid shl 8
    ld b, 3

.loop:
    rla
    rla
    rla
    ld (hl), a
    djnz .loop
    ld l, h
    ld (hl), 1

.wait:
    ld l, ti.spiStatus + 1

.wait1:
    ld a, (hl)
    and a, $f0
    jr nz, .wait1
    dec l

.wait2:
    bit 2, (hl)
    jr nz, .wait2
    ld l, h
    ld (hl), a
    ret
