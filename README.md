# Multi-Client-Server-Provider
Multi-Client&amp;Server&amp;Provider
CSE344 – System Programming – Final Project
DENİZ BABAT
131044013
Proje de istenenler;
1. client ve server
2. sinyal handler
3. Her bir provider'ın bir thread olması
4. server'ın istek atan clientları ilgili provider'lara ataması
5. logların basılması ve provider'ların dosyadan okunması
1. herbir provider'ın queue'sü olması ve size'ın 2 olması
2. ilgili provider yoksa ve server kapanırsa, pdf'te istenen mesajların
yollanması ve clientların buna göre aksiyon alması
3. herbir client'ın seperate process olması
4. Priority'lere göre client'ların providerlara dağıtılması
olarak tespit edilmiştir.

TASARIM

![Alt text](relative/path/to/img.jpg?raw=true "Title")

yukarıda çizildiği gibi program kodlanmıştır. Çizimde signal ve timer lar
gösterilmedi. Ama signal geldiği zaman clientlera mesaj yollanma bilgisi
belirtilmiştir. Duration zamanı gelen provider ölür. Daha sonra bu providera
artık iş atanmaz. Istenenler teker teker yapılmıştır. Herbir Client için kendi
pid'sine ait bir log dosyası “clientsdirlog” direktorisinin içine
bastırılmıştır. Eğer Client Serverdan cevap alamıyorsa “%s's task is not
completed. Sorry!” diye hem ekrana hemde dosyaya çıktı bastırıyorum.
Teşekkürler.
