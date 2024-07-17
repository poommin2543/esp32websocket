import asyncio
import websockets

async def handler(websocket, path):
    print(f"Client connected: {path}")
    try:
        async for message in websocket:
            print(f"Received message: {message}")
            response = "Hello from server"
            await websocket.send(response)
    except websockets.ConnectionClosed:
        print("Client disconnected")
    except Exception as e:
        print(f"Error: {e}")

async def main():
    async with websockets.serve(handler, "192.168.10.155", 5000):
        print("Server started on ws://192.168.10.155:5000")
        await asyncio.Future()  # Run forever

if __name__ == "__main__":
    asyncio.run(main())
