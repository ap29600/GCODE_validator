---
geometry:
    margin=30mm
---

# GCODE validator

un programma per analizzare un file GCODE e verificarne la compatibilità
con la propria stampante.

## Idea dell'interfaccia:

```
user@machine $ gcode_validator print.gcode

reading whitelist ........ done
opening file ............. done
checking file contents ...
>> Warning: Unexpected instruction G3: arc move
>> Warning: Dangerous instruction G6: direct stepper movement (machine-dependent behaviour)
>> Error: move results in out of bounds extruder position (line 21284 of original GCODE)
Stopping check due to unrecoverable position

user@machine $ gcode_validator print.gcode \
> --machine-geometry="0:0:0,400x400x400" --whitelist="instructions.gcode"

reading whitelist ........ done
opening file ............. done
checking file contents ... done
Check completed with no issues.

user@machine $ _
```

## Grafico di esecuzione / dipendenze

```{.graphviz}
digraph G { 
    size="3"
    node [shape=record];
    Src [ label = "Lettura file GCODE" ];
    Clean [ label = "Rimozione commenti\ne metadata" ];
    Parse [ label = "Parsing delle\nistruzioni" ];
    Scan [ label = "Scansione istruzioni\nillegali" ];
    Sym [ label = "Simulazione percorso\ne controllo limiti\ndella macchina" ];
    WhtLst [ label = "Parsing whitelist\ndell' utente|Deduzione da\nGCODE affidabile" ];
    Gen [ label = "Generazione file whitelist\nda quella in memoria"]

    Src -> Clean -> Parse -> Scan -> Sym;
    WhtLst -> Scan;
    WhtLst -> Gen [style = "dotted"];
}
```
