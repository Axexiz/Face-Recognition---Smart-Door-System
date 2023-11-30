import cv2
import pickle
import serial

# Setting serial port to COM3 at a baud rate of 9600
port = serial.Serial('COM3', 9600)

video = cv2.VideoCapture(0)
cascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")

recognize = cv2.face.LBPHFaceRecognizer_create()
recognize.read("trainner.yml")

labels = {}
with open("labels.pickle", 'rb') as f:
    og_label = pickle.load(f)
    labels = {v: k for k, v in og_label.items()}
    print(labels)

while True:
    check, frame = video.read() 
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    faces = cascade.detectMultiScale(gray, scaleFactor=1.2, minNeighbors=5)

    if len(faces) == 1:  # Check if there is exactly one face detected
        x, y, w, h = faces[0]
        face_save = gray[y:y + h, x:x + w]

        ID, conf = recognize.predict(face_save)

        if conf >= 20 and conf <= 115:
            if ID == 0:
                port.write(str.encode('0'))
                print("sent 0")
                cv2.putText(frame, labels[ID], (x - 10, y - 10), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 18, 5), 2, cv2.LINE_AA)
                cv2.waitKey(200)
            else:
                port.write(str.encode('1'))
                print("sent 1")
                cv2.putText(frame, labels[ID], (x - 10, y - 10), cv2.FONT_HERSHEY_COMPLEX, 1, (18, 5, 255), 2, cv2.LINE_AA)
                cv2.waitKey(200)

        frame = cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 4)

    cv2.imshow("Video", frame)
    key = cv2.waitKey(1)
    if key == ord('q'):
        break

video.release()
cv2.destroyAllWindows()
