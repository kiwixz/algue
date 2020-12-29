export default {
  async live_game(summoner: string) {
    return this.get("live_game", { summoner });
  },
  
  async get(url: string, params: object) {
    let res = await fetch(`/api/${url}`, {
      method: "POST",
      headers: { "content-type": "application/json" },
      body: JSON.stringify(params),
    });

    if (!res.ok) {
      if (res.headers.get("content-type") === "application/json") {
        let message = (await res.json()).message;
        throw new Error(`Backend API error: ${message}`);
      } else {
        throw new Error(`Backend API HTTP error: ${res.status} ${res.statusText}`);
      }
    }

    return await res.json();
  },
};
