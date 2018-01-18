.global setLidt
.extern idtPointers
 setLidt:
        lidt idtPointers
        ret
