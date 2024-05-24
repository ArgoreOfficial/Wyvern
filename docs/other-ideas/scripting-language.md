# Object System
### iScriptableObject
Basic scriptable object interface.  
This object can contain one or more script.

### cVirtualMachine
Backend system taking care of exec

# Scripting Language

```
ClassName Player 

Property Int maxHealth                 ; set in the editor

Event OnCreate()
{
    Health = maxHealth                 ; variable on the parent object
    MoveToSpawnLocation()       ; some other function
}
```

