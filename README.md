# projetos_spectre

1. Objectif du programme

Le programme réalise un analyseur de spectre audio temps réel sur la carte STM32F746G-DISCO.

Son rôle est de :

Capturer le son provenant du jack audio 3.5 mm.
Numériser ce son à 44,1 kHz.
Calculer sa Transformée de Fourier Rapide (FFT).
Afficher simultanément :
la forme d'onde du signal (oscilloscope)
le spectre fréquentiel (barres FFT colorées).


3. Logique générale du système

Le programme suit la chaîne de traitement suivante :

Signal Audio >>>Codec WM8994 >>> SAI2 >>> DMA >>> Buffer Audio >>> Prétraitement >>> FFT >>> Calcul des amplitudes>>> Affichage LCD

Le principe est :
  
1.Le matériel récupère le son.
2.Le DMA stocke les échantillons sans utiliser le CPU.
3.Le processeur analyse les données reçues.
4.Les résultats sont affichés en temps réel.

3. Taille de la FFT

Le programme utilise une FFT de 1024 points. À chaque traitement, un bloc de 1024 échantillons est analysé pour extraire son contenu fréquentiel. Ce choix représente un compromis entre la précision fréquentielle et la rapidité d'exécution. Avec une fréquence d'échantillonnage de 44,1 kHz, la résolution fréquentielle obtenue est d'environ 43 Hz par bin, ce qui permet de distinguer correctement les différentes composantes du signal audio.

4.Utilisation du DMA

Le transfert des données audio vers la mémoire est assuré par le contrôleur DMA (Direct Memory Access). Cette technique permet de transférer les échantillons directement depuis le périphérique audio vers la mémoire RAM sans solliciter le processeur. Ainsi, le microcontrôleur reste disponible pour effectuer les calculs FFT et l'affichage graphique, ce qui améliore considérablement les performances du système en temps réel.

5.Technique du Double Buffering

Afin d'éviter toute perte de données pendant l'acquisition audio, le programme utilise une technique appelée double buffering. Le buffer mémoire est divisé en deux parties. Pendant que le DMA remplit une moitié du buffer avec de nouvelles données audio, le processeur traite simultanément l'autre moitié. Cette méthode garantit une acquisition continue du signal tout en permettant son traitement en temps réel.

6.Gestion des interruptions DMA

Le DMA génère des interruptions lorsque la moitié ou la totalité du buffer est remplie. Ces interruptions ne réalisent aucun traitement complexe ; elles se contentent de positionner des indicateurs appelés flags. Cette approche permet de maintenir des interruptions très courtes et de réserver les calculs lourds, tels que la FFT et l'affichage graphique, à la boucle principale du programme. Cette organisation constitue une bonne pratique dans les systèmes embarqués temps réel.

7.Initialisation du système d'affichage

Lors du démarrage du programme, le module d'affichage LCD est initialisé afin de préparer l'écran pour l'affichage graphique. Le système configure les couleurs, les polices ainsi que les différentes zones d'affichage. Une zone supérieure est réservée à la représentation temporelle du signal tandis qu'une zone inférieure est utilisée pour afficher le spectre fréquentiel obtenu après calcul de la FFT.

8.Application de la fenêtre de Hanning

Avant d'effectuer la FFT, le programme applique une fenêtre de Hanning aux échantillons audio. Cette opération consiste à multiplier chaque échantillon par un coefficient calculé selon une fonction mathématique spécifique. L'objectif est de réduire les effets de bord liés à la découpe du signal en blocs, ce qui diminue les fuites spectrales et améliore la précision de l'analyse fréquentielle.

9.Calcul de la Transformée de Fourier Rapide

La FFT est réalisée grâce à la bibliothèque CMSIS-DSP optimisée pour les processeurs ARM Cortex-M7. Cette opération transforme le signal du domaine temporel vers le domaine fréquentiel. Le résultat permet de déterminer quelles fréquences sont présentes dans le signal ainsi que leur amplitude respective. Cette étape constitue le cœur du fonctionnement de l'analyseur de spectre.


10.Calcul des amplitudes fréquentielles

Après le calcul de la FFT, le programme détermine l'amplitude de chaque composante fréquentielle. Pour cela, il calcule le module des valeurs complexes obtenues à la sortie de la FFT. Ces amplitudes représentent l'énergie contenue dans chaque fréquence du signal audio et servent ensuite à construire l'affichage du spectre.

11.Conversion en décibels

Les amplitudes calculées sont converties en décibels à l'aide de la formule logarithmique 20·log10(x). Cette conversion permet d'obtenir une représentation plus proche de la perception humaine du son.

12.Affichage du spectre fréquentiel

La partie inférieure de l'écran affiche le spectre audio sous forme de 60 barres verticales. La hauteur de chaque barre est proportionnelle à l'amplitude de la bande fréquentielle correspondante. Les couleurs évoluent progressivement du vert vers le rouge, permettant d'identifier visuellement les différentes zones du spectre, des basses fréquences aux hautes fréquences.

13.Lissage de l'affichage

Afin d'améliorer le confort visuel, le programme applique un algorithme de lissage aux hauteurs des barres du spectre. Les barres montent rapidement lorsqu'une fréquence apparaît mais redescendent plus lentement lorsqu'elle disparaît.

14.Boucle principale du programme

La boucle principale assure la surveillance continue des indicateurs générés par le DMA. Lorsqu'une moitié du buffer est disponible, les données correspondantes sont immédiatement traitées par le module FFT puis affichées à l'écran. Grâce au double buffering, cette opération peut être réalisée en continu sans interrompre l'acquisition audio. Le système fonctionne ainsi en temps réel avec une faible charge processeur et sans perte d'échantillons.

Conclusion
L'architecture du programme repose sur une combinaison efficace entre acquisition audio, traitement numérique du signal et affichage graphique. L'utilisation du DMA, du double buffering et de la bibliothèque CMSIS-DSP permet de réaliser une analyse fréquentielle en temps réel tout en garantissant des performances élevées sur le microcontrôleur STM32F746G-DISCO. Le système offre ainsi une visualisation claire et instantanée des caractéristiques temporelles et fréquentielles du signal audio.

