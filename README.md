## Installation :

```bash
pip install -r requirements.txt
```

Utilisation avec des images données dans l'ordre :

```bash
Prototype python

python main.py img1.jpg img2.jpg img3.jpg ...


Pour vérifier la qualité du prototype en fonction des critères choisis

python evaluate.py img1.jpg img2.jpg img3.jpg ... 


Pour effectuer un benchmark entre la version cpp et python

python benchmark.py img1.jpg img2.jpg img3.jpg ...


Pour vérifier la cohérence du prototype avec la version cpp

python verify_consisttency.py img1.jpg img2.jpg img3.jpg ...

----------------------------------------------------------------

Version C++
Pour compiler et run le projet:

make
./bin/panorama img1.jpg img2.jpg img3.jpg ...

Pour effectuer les tests :
make test

```

## Documents
[État de l’art](https://drive.google.com/file/d/1_4qmunYhaGeXiwZbAOKs1EZPpBRFHnf3/view?usp=drive_link)  
[Critères d’évaluation](https://drive.google.com/file/d/1KRuBSmMtwYqvL51lyxIU8_h5fe374KJ7/view?usp=sharing)  
[Base de données](https://drive.google.com/drive/folders/1kC7KAULd5mZsqaWnY3-rSbQLaZ7LujTY)  