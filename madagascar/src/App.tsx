
import { useState } from "react";

const METHODS = ["GET", "POST", "PUT", "DELETE"] as const;
type Method = (typeof METHODS)[number];

const METHOD_COLORS: Record<Method, string> = {
  GET: "bg-emerald-500 hover:bg-emerald-400",
  POST: "bg-sky-500 hover:bg-sky-400",
  PUT: "bg-amber-500 hover:bg-amber-400",
  DELETE: "bg-rose-500 hover:bg-rose-400",
};

function App() {
  const [active, setActive] = useState<Method>("GET");
  const [path, setPath] = useState("/");
  const [body, setBody] = useState("");
  const [response, setResponse] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  const handleSend = async () => {
    setLoading(true);
    setResponse(null);
    try {
      const options: RequestInit = { method: active };
      if (active === "POST" || active === "PUT") {
        options.body = body;
        options.headers = { "Content-Type": "text/plain" };
      }
      const res = await fetch(`http://localhost:8080${path}`, options);
      const text = await res.text();
      setResponse(`${res.status} ${res.statusText}\n\n${text}`);
    } catch (err) {
      setResponse(`Error: ${String(err)}`);
    } finally {
      setLoading(false);
    }
  };

  return (
      <div className="min-h-screen bg-[#1a1a2e] text-white font-mono flex">
        {/* Sidebar */}
        <aside className="w-56 bg-[#16162a] border-r border-white/10 flex flex-col p-6 gap-6">
          <div>
            <h1 className="text-2xl font-bold tracking-tight text-white">
              webserv
            </h1>
            <p className="text-[11px] text-white/30 mt-1">HTTP/1.1 — Codam</p>
          </div>

          <div className="flex flex-col gap-2 mt-4">
          <span className="text-[10px] uppercase tracking-widest text-white/40 mb-1">
            Method
          </span>
            {METHODS.map((m) => (
                <button
                    key={m}
                    onClick={() => setActive(m)}
                    className={`text-left px-3 py-2 rounded text-sm font-semibold transition-all ${
                        active === m
                            ? `${METHOD_COLORS[m]} text-white shadow-lg`
                            : "bg-white/5 text-white/50 hover:text-white hover:bg-white/10"
                    }`}
                >
                  {m}
                </button>
            ))}
          </div>

          <div className="mt-auto text-[10px] text-white/20 leading-relaxed">
            42 Network Project
            <br />
            madaglia × fkoolhov
          </div>
        </aside>

        {/* Main Content */}
        <main className="flex-1 flex flex-col p-10 gap-6 max-w-2xl">
          <div>
          <span className="text-[10px] uppercase tracking-widest text-white/40">
            Request
          </span>
            <div className="flex gap-3 mt-2 items-center">
            <span
                className={`text-xs font-bold px-2 py-1 rounded ${METHOD_COLORS[active]}`}
            >
              {active}
            </span>
              <input
                  className="flex-1 bg-white/5 border border-white/10 rounded px-4 py-2 text-sm focus:outline-none focus:border-white/30 placeholder-white/20"
                  value={path}
                  onChange={(e) => setPath(e.target.value)}
                  placeholder="/index.html"
              />
              <button
                  onClick={handleSend}
                  disabled={loading}
                  className="bg-white text-[#1a1a2e] font-bold px-6 py-2 rounded text-sm hover:bg-white/90 transition disabled:opacity-50"
              >
                {loading ? "..." : "Send"}
              </button>
            </div>
          </div>

          {(active === "POST" || active === "PUT") && (
              <div>
            <span className="text-[10px] uppercase tracking-widest text-white/40">
              Body
            </span>
                <textarea
                    className="w-full mt-2 bg-white/5 border border-white/10 rounded px-4 py-3 text-sm focus:outline-none focus:border-white/30 placeholder-white/20 h-32 resize-none"
                    value={body}
                    onChange={(e) => setBody(e.target.value)}
                    placeholder="Request body..."
                />
              </div>
          )}

          {response !== null && (
              <div>
            <span className="text-[10px] uppercase tracking-widest text-white/40">
              Response
            </span>
                <pre className="mt-2 bg-black/30 border border-white/10 rounded p-4 text-xs whitespace-pre-wrap break-all leading-relaxed max-h-96 overflow-auto">
              {response}
            </pre>
              </div>
          )}
        </main>
      </div>
  );
}

export default App;