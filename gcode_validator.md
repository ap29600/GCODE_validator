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
    Machine [label = "Costruzione di un\nprofilo macchina" ]
    Gen [ label = "Generazione file whitelist\nda quella in memoria"]

    Src -> Clean -> Parse -> Machine 
    WhtLst -> Machine
    WhtLst -> Gen [style = "dotted"]
    Machine -> Scan
    Machine -> Sym
}
```

## Controlli di simulazione

Durante il controllo dinamico del GCODE, il programma controlla le seguenti
condizioni:

- Legalità delle istruzioni.
- Compatibilità dei movimenti con la geometria della macchina.
- Assenza di collisione del carrello con elementi stampati.
- Settaggio della temperatura dell'estrusore in valori consentiti.
- Conformità del rapporto tra distanza percorsa e plastica estrusa.

Per effettuare questi controlli l'interfaccia può presentare le seguenti componenti:

```cpp

class Validator { /* .. */ };

// a and b are antipodal vertices of the box.
typedef struct {
    Vec3 a, b;
} Box;

typedef struct {
    Box  limits;
    Vec3 position;
    Vec3 zero_pos;
} Extruder;

class Machine {
public:
    Machine();
    enum MachineResult consume_instruction(Instruction); 
private:
    std::Vec<Extruder> extruders;
    std::Vec<Box>      extruded;   
    Validator          validator;
};

```
